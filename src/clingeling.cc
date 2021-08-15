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

struct EventPipe {
public:
	SignalProxy<void(void)> & on_event{on_event_};
	SlotProxy<void(EPoll::Events const&)> & on_fd_event{on_fd_event_};

	explicit EventPipe(Posix::Pipe const& pipe)
	:
		pipe_{pipe},
		on_fd_event_{[this](auto const& ev) { handle_fd_event(ev); }}
	{ }

	void trigger()
	{
		int wakeup{0};
		std::get<1>(pipe_)->write(&wakeup, sizeof(wakeup));
	}

	std::shared_ptr<Posix::Fd> fd() const
	{
		return std::get<0>(pipe_);
	}

	EPoll::Events ev() const
	{
		return EPoll::Events{EPoll::Event::In};
	}

private:
	void handle_fd_event(EPoll::Events const& ev)
	{
		if (ev != EPoll::Event::In) {
			throw std::runtime_error("bad epoll event");
		}

		int wakeup{0};
		auto res = std::get<0>(pipe_)->read(&wakeup, sizeof(wakeup));
		if (res == sizeof(wakeup)) {
			on_event_();
			return;
		}
		throw std::runtime_error("bad read from pipe");
	}

	Posix::Pipe pipe_;
	Signal<void(void)> on_event_;
	Slot<void(EPoll::Events const&)> on_fd_event_;
};

int clingeling(int, char *[])
{
	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	auto socket_factory = Posix::SocketFactory::create();

	auto socket_buffer = BufferedStreamSocket(
		*poller, *socket_factory, Posix::SocketAddress{Posix::Inet::Address{"127.0.0.1"}, 4444});

	auto baresip_ctrl = Baresip::Ctrl::create(socket_buffer.recvbuf(), socket_buffer.sendbuf());

	auto baresip_model = Baresip::Model::create();
	connect(baresip_model->on_event, baresip_ctrl->on_event);

	auto pipe_factory = Posix::PipeFactory::create();
	auto pipe = pipe_factory->make_pipe(Posix::PipeFactory::Params{false, true});

	EventPipe wakeup(pipe);
	auto run{true};
	wakeup.on_event.connect([&run]() { run = false; });
	poller->add(wakeup.fd(), wakeup.ev(), [&wakeup](auto const& ev) { wakeup.on_fd_event(ev); });

	try {
		do {
			poller->wait();
		} while (run);
	} catch (...) {
		throw_backtrace();
	}

	return 0;
}
