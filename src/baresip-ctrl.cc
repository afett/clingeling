/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/
#include "baresip/ctrl.h"

#include "json/parser.h"
#include "netstring/reader.h"
#include "io/stream-buffer.h"
#include "io/event-buffer.h"

#include <sstream>

namespace Baresip {

class CtrlImpl : public Ctrl {
public:
	explicit CtrlImpl(IO::ReadEventBuffer &, IO::WriteBuffer &);
	void on_event(std::function<void(Event const&)> const&) final;

private:
	void on_json(Json::Object const&);

	IO::StreamBuffer recvbuf_;
	Netstring::Reader netstring_;
	IO::WriteBuffer & sendbuf_;
	std::function<void(Event const&)> on_event_;
};

std::unique_ptr<Ctrl> Ctrl::create(IO::ReadEventBuffer & recvbuf, IO::WriteBuffer & sendbuf)
{
	return std::make_unique<CtrlImpl>(recvbuf, sendbuf);
}

CtrlImpl::CtrlImpl(IO::ReadEventBuffer & recvbuf, IO::WriteBuffer & sendbuf)
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

void CtrlImpl::on_event(std::function<void(Event const&)> const& cb)
{
	on_event_ = cb;
}

namespace {

template <typename T>
T select(std::string const& str, std::initializer_list<std::tuple<std::string, T>> map)
{
	for (auto const& mapping : map) {
		if (str == std::get<0>(mapping)) {
			return std::get<1>(mapping);
		}
	}

	throw std::runtime_error("failed to map: " + str);
}

Event::Type event_type(std::string const& str)
{
	return select<Event::Type>(str, {
		{"REGISTER_OK", Event::Type::RegisterOk},
		{"REGISTER_FAIL", Event::Type::RegisterFail},
		{"UNREGISTERING", Event::Type::Unregistering},
	});
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

}

void CtrlImpl::on_json(Json::Object const& obj)
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
	}

	on_event_(ev);
}

}
