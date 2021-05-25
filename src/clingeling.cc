/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/
#include "epoll/ctrl.h"
#include "fmt.h"
#include "posix/socket.h"
#include "posix/inet-address.h"
#include "posix/socket-address.h"
#include "posix/pipe-factory.h"
#include "buffered-stream-socket.h"
#include "baresip/ctrl.h"

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

std::ostream & operator<<(std::ostream & os, Baresip::Event::Register::Type type)
{
	switch (type) {
	case Baresip::Event::Register::Type::Fail: return os << "Fail";
	case Baresip::Event::Register::Type::Ok:  return os << "Ok";
	case Baresip::Event::Register::Type::Unregistering: return os << "Unregistering";
	}
	return os;
}

std::ostream & operator<<(std::ostream & os, Baresip::Event::Call::Type type)
{
	switch (type) {
	case Baresip::Event::Call::Type::Established: return os << "Established";
	case Baresip::Event::Call::Type::Incoming: return os << "Incoming";
	case Baresip::Event::Call::Type::Ringing: return os << "Ringing";
	case Baresip::Event::Call::Type::Closed: return os << "Closed";
	}
	return os;
}

std::ostream & operator<<(std::ostream & os, Baresip::Event::Call::Direction direction)
{
	switch (direction) {
	case Baresip::Event::Call::Direction::Incoming: return os << "Incoming";
	case Baresip::Event::Call::Direction::Outgoing: return os << "Outgoing";
	}
	return os;
}

namespace {

struct EventLogger {
	void operator()(Baresip::Event::Register const& ev)
	{
		std::cerr << Fmt::format("Event: Register: %s, %s, '%s'", ev.type, ev.accountaor, ev.param) << "\n";
	}

	void operator()(Baresip::Event::Call const& ev)
	{
		std::cerr << Fmt::format("Event: Call: %s, %s, %s, %s, %s, '%s'", ev.type, ev.accountaor, ev.direction, ev.peeruri, ev.id, ev.param) << "\n";
	}
};

}

int clingeling(int, char *[])
{
	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	auto socket_factory = Posix::SocketFactory::create();

	auto socket_buffer = BufferedStreamSocket(
		*poller, *socket_factory, Posix::SocketAddress{Posix::Inet::Address{"127.0.0.1"}, 4444});

	auto baresip_ctrl = Baresip::Ctrl::create(socket_buffer.recvbuf(), socket_buffer.sendbuf());
	baresip_ctrl->on_event([](auto ev) { std::visit(::EventLogger{}, ev); });
	baresip_ctrl->on_response([](auto const&) { std::cerr << "unexpected response object\n"; });

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
