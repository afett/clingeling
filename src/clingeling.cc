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

int clingeling(int, char *[])
{
	auto socket_factory = Posix::SocketFactory::create();
	auto socket = socket_factory->make_stream_socket({
			Posix::SocketFactory::Params::Domain::Inet,
			Posix::SocketFactory::Params::Type::Stream,
			Posix::Fd::Option::nonblock|Posix::Fd::Option::cloexec});

	socket->connect(Posix::SocketAddress{Posix::Inet::Address{"127.0.0.1"}, 4444});

	std::cerr << socket->state() << "\n";

	EPoll::Events ev{EPoll::Event::In};
	switch (socket->state()) {
	case Posix::StreamSocket::State::in_progress:
		ev = EPoll::Event::Out;
		// fallthrough
	case Posix::StreamSocket::State::connected:
		break;
	case Posix::StreamSocket::State::error:
		throw std::runtime_error("Socket in state Error");
	case Posix::StreamSocket::State::init:
		throw std::runtime_error("Socket in state Init");
	}

	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};
	auto netstring = Netstring::Reader{stream};

	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	poller->add(socket, ev, [&netstring, &socket, &buf, &poller](auto ev) {
		if (!ev) {
			return;
		}

		if (socket->state() == Posix::StreamSocket::State::in_progress) {
			socket->connect_continue();
			poller->mod(socket, EPoll::Events{EPoll::Event::In});
			return;
		}

		if (ev & EPoll::Event::Out) {
			throw std::runtime_error("POLLOUT");
		}
		if (ev & EPoll::Event::RdHup) {
			throw std::runtime_error("RDHUP");
		}
		if (ev & EPoll::Event::Pri) {
			throw std::runtime_error("PRI");
		}
		if (ev & EPoll::Event::Err) {
			throw std::runtime_error("ERR");
		}
		if (ev & EPoll::Event::Hup) {
			throw std::runtime_error("HUP");
		}
		if (ev != EPoll::Event::In) {
			throw std::runtime_error("bad epoll event");
		}

		auto res = socket->read(buf.wstart(), buf.wsize());
		if (res == 0) {
			// FIXME
			throw std::runtime_error("Connection closed");
		}

		auto json = std::string{};
		if (netstring.parse(json)) {
			std::cout << json << "\n";
		}

		poller->mod(socket, buf.full() ? EPoll::Events{} : EPoll::Events{EPoll::Event::In});
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
