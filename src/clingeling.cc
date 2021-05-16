/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include "epoll/ctrl.h"
#include "posix/socket.h"
#include "posix/inet-address.h"
#include "posix/socket-address.h"
#include "io/buffer.h"
#include "io/event-buffer.h"
#include "io/stream-buffer.h"
#include "posix/fd.h"
#include "posix/pipe-factory.h"
#include "fmt.h"
#include "netstring/reader.h"
#include <system_error>
#include <iostream>

std::ostream & operator<<(std::ostream & os, Posix::StreamSocket::State state)
{
	switch (state) {
	case Posix::StreamSocket::State::init: return os << "init";
	case Posix::StreamSocket::State::in_progress: return os << "in_progress";
	case Posix::StreamSocket::State::connected: return os << "connected";
	case Posix::StreamSocket::State::error: return os << "error";
	}
	return os;
}

class BufferedStreamSocket {
public:
	BufferedStreamSocket(
		EPoll::Ctrl & poller,
		Posix::SocketFactory & socket_factory,
		Posix::SocketAddress const& addr)
	:
		sendbuf_{4096},
		recvbuf_{4096},
		poller_(poller),
		socket_(socket_factory.make_stream_socket({
			Posix::SocketFactory::Params::Domain::Inet,
			Posix::SocketFactory::Params::Type::Stream,
			Posix::Fd::Option::nonblock|Posix::Fd::Option::cloexec}))
	{
		connect(addr);

		recvbuf_.on_drain([this] () { update_poll_events(); });

		poller.add(socket_, poll_events(), [this] (auto ev) {
			if (!ev) {
				return;
			}

			if (socket_->state() == Posix::StreamSocket::State::in_progress) {
				socket_->connect_continue();
				update_poll_events();
				return;
			}

			dispatch(ev, {
				{EPoll::Event::RdHup, [] () { throw std::runtime_error("RDHUP"); }},
				{EPoll::Event::Pri,   [] () { throw std::runtime_error("PRI"); }},
				{EPoll::Event::Err,   [] () { throw std::runtime_error("ERR"); }},
				{EPoll::Event::Hup,   [] () { throw std::runtime_error("HUP"); }},
				{EPoll::Event::In,    [this] () { on_readable(); }},
				{EPoll::Event::Out,   [this] () { on_writable(); }}
			});

			update_poll_events();
		});
	}

	IO::ReadEventBuffer & recvbuf()
	{
		return recvbuf_;
	}

	IO::WriteBuffer & sendbuf()
	{
		return sendbuf_;
	}

private:
	void on_readable()
	{
		auto size = socket_->read(recvbuf_.wstart(), recvbuf_.wsize());
		if (size == 0) {
			// FIXME
			throw std::runtime_error("Connection closed");
		}
		recvbuf_.fill(size);
	}

	void on_writable()
	{
		auto size = socket_->write(sendbuf_.rstart(), sendbuf_.rsize());
		sendbuf_.drain(size);
	}

	void connect(Posix::SocketAddress const& addr)
	{
		socket_->connect(addr);

		switch (socket_->state()) {
		case Posix::StreamSocket::State::init:
			throw std::runtime_error("Socket in state Init");
		case Posix::StreamSocket::State::error:
			throw std::runtime_error("Socket in state Error");
		case Posix::StreamSocket::State::in_progress:
		case Posix::StreamSocket::State::connected:
			break;
		}
	}

	void update_poll_events()
	{
		poller_.mod(socket_, poll_events());
	}

	EPoll::Events poll_events() const
	{
		if (socket_->state() == Posix::StreamSocket::State::in_progress) {
			return EPoll::Events{EPoll::Event::Out};
		}

		EPoll::Events ev;
		if (!sendbuf_.empty()) {
			ev |= EPoll::Event::Out;
		}
		if (!recvbuf_.full()) {
			ev |= EPoll::Event::In;
		}
		return ev;
	}

	void dispatch(
		EPoll::Events const& ev,
		std::initializer_list<std::tuple<EPoll::Event, std::function<void(void)>>> actions)
	{
		for (auto const& action : actions) {
			if (ev & std::get<0>(action)) {
				std::get<1>(action)();
			}
		}
	}

	IO::EventBuffer sendbuf_;
	IO::EventBuffer recvbuf_;
	EPoll::Ctrl & poller_;
	std::shared_ptr<Posix::StreamSocket> socket_;
};

int clingeling(int, char *[])
{
	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	auto socket_factory = Posix::SocketFactory::create();

	auto socket_buffer = BufferedStreamSocket(
		*poller, *socket_factory, Posix::SocketAddress{Posix::Inet::Address{"127.0.0.1"}, 4444});

	auto stream = IO::StreamBuffer{socket_buffer.recvbuf()};
	auto netstring = Netstring::Reader{stream};

	socket_buffer.recvbuf().on_fill([&netstring] () {
		auto json = std::string{};
		if (netstring.parse(json)) {
			std::cout << json << "\n";
		}
	});

	auto pipe_factory = Posix::PipeFactory::create();
	auto pipe = pipe_factory->make_pipe(Posix::PipeFactory::Params{false, true});

	auto run{true};
	poller->add(std::get<0>(pipe), EPoll::Events{EPoll::Event::In}, [&run, &pipe](auto ev) {
		if (ev != EPoll::Event::In) {
			throw std::runtime_error("bad epoll event");
		}

		int wakeup{0};
		auto res = std::get<0>(pipe)->read(&wakeup, sizeof(wakeup));
		if (res == sizeof(wakeup)) {
			run = false;
			return;
		}
		throw std::runtime_error("bad read from pipe");
	});

	do {
		poller->wait();
	} while (run);

	return 0;
}
