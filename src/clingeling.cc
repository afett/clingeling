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

#include "json/parser.h"

#include <system_error>
#include <iostream>
#include <sstream>

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

namespace Baresip {
class Event {
public:
	enum class Type {
		RegisterFail,
	};

	enum class Class {
		Register,
	};

	Type type;
	Class klass;
	std::string accountaor;
	std::string param;
};

Event::Type event_type(std::string const&)
{
	return Event::Type::RegisterFail;
}

Event::Class event_class(std::string const&)
{
	return Event::Class::Register;
}

template <typename T>
std::add_pointer_t<const T> get_if(Json::Object const* obj, const char* name)
{
	auto member = obj->find(std::string(name));
	if (member == std::end(*obj)) {
		return nullptr;
	}

	return Json::get_if<T>(&member->second);
}

bool is_event(Json::Object const& obj)
{
	if (auto event = get_if<bool>(&obj, "event")) {
		return *event;
	}
	return false;
}

class Ctrl {
public:
	explicit Ctrl(IO::ReadEventBuffer & recvbuf, IO::WriteBuffer & sendbuf)
	:
		recvbuf_(recvbuf),
		netstring_(recvbuf_),
		sendbuf_(sendbuf)
	{
		recvbuf.on_fill([this] () {
			auto data = std::string{};
			if (netstring_.parse(data)) {
				std::stringstream ss{data};
				on_json(Json::parse_object(ss));
			}
		});
	}

	void on_event(std::function<void(Event const&)> const& cb)
	{
		on_event_ = cb;
	}

private:
	void on_json(Json::Object const& obj)
	{
		if (!is_event(obj)) {
			return;
		}

		Event ev;
		if (auto type_str = get_if<std::string>(&obj, "type")) {
			ev.type = event_type(*type_str);
		} else {
			throw std::runtime_error("failed to get event type");
		}

		if (auto class_str = get_if<std::string>(&obj, "class")) {
			ev.klass = event_class(*class_str);
		} else {
			throw std::runtime_error("failed to get event class");
		}

		if (auto accountaor = get_if<std::string>(&obj, "accountaor")) {
			ev.accountaor = *accountaor;
		} else {
			throw std::runtime_error("failed to get event accountaor");
		}

		if (auto param = get_if<std::string>(&obj, "param")) {
			ev.param = *param;
		} else {
			throw std::runtime_error("failed to get event param");
		}

		on_event_(ev);
	}

	IO::StreamBuffer recvbuf_;
	Netstring::Reader netstring_;
	IO::WriteBuffer & sendbuf_;
	std::function<void(Event const&)> on_event_;
};

}

int clingeling(int, char *[])
{
	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	auto socket_factory = Posix::SocketFactory::create();

	auto socket_buffer = BufferedStreamSocket(
		*poller, *socket_factory, Posix::SocketAddress{Posix::Inet::Address{"127.0.0.1"}, 4444});

	auto baresip_ctrl = Baresip::Ctrl{socket_buffer.recvbuf(), socket_buffer.sendbuf()};

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
