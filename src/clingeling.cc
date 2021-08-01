/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/
#include "epoll/ctrl.h"
#include "posix/socket.h"
#include "posix/inet-address.h"
#include "posix/socket-address.h"
#include "posix/pipe-factory.h"
#include "buffered-stream-socket.h"
#include "baresip/ctrl.h"
#include "baresip/model.h"
#include "source-location.h"

int clingeling(int, char *[])
{
	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	auto socket_factory = Posix::SocketFactory::create();

	auto socket_buffer = BufferedStreamSocket(
		*poller, *socket_factory, Posix::SocketAddress{Posix::Inet::Address{"127.0.0.1"}, 4444});

	auto baresip_ctrl = Baresip::Ctrl::create(socket_buffer.recvbuf(), socket_buffer.sendbuf());

	auto baresip_model = Baresip::Model::create();
	baresip_ctrl->on_event([&baresip_model](auto const& ev) { baresip_model->on_event(ev); });

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

	try {
		do {
			poller->wait();
		} while (run);
	} catch (...) {
		throw_backtrace();
	}

	return 0;
}
