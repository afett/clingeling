/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/
#include "baresip-event-parser.h"

namespace Baresip {

namespace {

template <typename T>
std::tuple<bool, T> select(std::string const& str, std::initializer_list<std::tuple<std::string, T>> map)
{
	for (auto const& mapping : map) {
		if (str == std::get<0>(mapping)) {
			return {true, std::get<1>(mapping)};
		}
	}

	return {false, T{}};
}

std::tuple<bool, Event::Register::Type> event_type(std::string const& str)
{
	return select<Event::Register::Type>(str, {
		{"REGISTER_OK", Event::Register::Type::Ok},
		{"REGISTER_FAIL", Event::Register::Type::Fail},
		{"UNREGISTERING", Event::Register::Type::Unregistering},
	});
}

std::tuple<bool, Event::Class> event_class(std::string const& str)
{
	return select<Event::Class>(str, {
		{"register", Event::Class::Register},
	});
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

Event::Register parse_register_event(Json::Object const& obj)
{
	Event::Register ev;
	bool ok{false};
	if (auto type_str = get_if<std::string>(&obj, "type")) {
		std::tie(ok, ev.type) = event_type(*type_str);
		if (!ok) {
			throw std::runtime_error("unknown event type");
		}
	} else {
		throw std::runtime_error("failed to get event type");
	}

	if (auto accountaor = get_if<std::string>(&obj, "accountaor")) {
		ev.accountaor = *accountaor;
	} else {
		throw std::runtime_error("failed to get event accountaor");
	}

	if (auto param = get_if<std::string>(&obj, "param")) {
		ev.param = *param;
	}

	return ev;
}

}


std::pair<bool, Event::Any> parse_event(Json::Object const& obj)
{
	if (!is_event(obj)) {
		return {false, Event::Any{}};
	}

	if (auto class_str = get_if<std::string>(&obj, "class")) {
		auto [ok, klass] = event_class(*class_str);
		if (!ok) {
			return {false, {}};
		}
	} else {
		throw std::runtime_error("failed to get event class");
	}

	return {true, parse_register_event(obj)};
}

}

