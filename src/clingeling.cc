/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/
#include "epoll/ctrl.h"
#include "posix/socket.h"
#include "posix/inet-address.h"
#include "posix/socket-address.h"
#include "io/stream-buffer.h"
#include "posix/pipe-factory.h"
#include "buffered-stream-socket.h"
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
