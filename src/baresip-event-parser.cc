/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/
#include "baresip-event-parser.h"

namespace Baresip {
namespace Event {

namespace {

template <typename K, typename KK, typename V, typename ...Args>
std::tuple<bool, V> select_helper(K const& cmp, KK const& key, V const& res, Args ...args)
{
	if (cmp == key) {
		return {true, res};
	}

	if constexpr (sizeof...(args)) {
		return select_helper(cmp, args...);
	}

	return {false, {}};
}

template <typename K, typename ...Args>
auto select(K const& cmp, Args ...args) -> decltype(select_helper(cmp, args...))
{
	return select_helper(cmp, args...);
}

std::tuple<bool, Register::Type> event_type(std::string const& str)
{
	return select(str,
		"REGISTER_OK", Register::Type::Ok,
		"REGISTER_FAIL", Register::Type::Fail,
		"UNREGISTERING", Register::Type::Unregistering
	);
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
	auto is_ev = get_if<bool>(&obj, "event");
	return is_ev ? *is_ev : false;
}

Register parse_register_event(Json::Object const& obj)
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

std::tuple<bool, Any> parse(Json::Object const& obj)
{
	if (!is_event(obj)) {
		return {false, Event::Any{}};
	}

	if (auto class_str = get_if<std::string>(&obj, "class")) {
		auto [ok, parser] = select(*class_str,
			"register", &parse_register_event
		);

		if (ok) {
			return {true, parser(obj)};
		}
	} else {
		throw std::runtime_error("failed to get event class");
	}

	return {false, {}};
}

}}
