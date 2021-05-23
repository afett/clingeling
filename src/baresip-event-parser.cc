/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/
#include "baresip-event-parser.h"

#include <functional>

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

template <typename T>
std::tuple<bool, T> get_member(Json::Object const& obj, std::string const& name)
{
	auto member = obj.find(std::string(name));
	if (member == std::end(obj)) {
		return {false, {}};
	}

	auto value = Json::get_if<T>(&member->second);
	if (!value) {
		return {false, {}};
	}

	return {true, *value};
}

bool is_event(Json::Object const& obj)
{
	auto is_ev = get_member<bool>(obj, "event");
	return std::get<0>(is_ev) && std::get<1>(is_ev);
}

Register parse_register_event(Json::Object const& obj)
{
	Event::Register ev;
	auto [ok, type_str] = get_member<std::string>(obj, "type");
	if (!ok) {
		throw std::runtime_error("failed to get register event type");
	}
	std::tie(ok, ev.type) = select(type_str,
		"REGISTER_OK", Register::Type::Ok,
		"REGISTER_FAIL", Register::Type::Fail,
		"UNREGISTERING", Register::Type::Unregistering
	);
	if (!ok) {
		throw std::runtime_error("failed to translate register event type");
	}

	std::tie(ok, ev.accountaor) = get_member<std::string>(obj, "accountaor");
	if (!ok) {
		throw std::runtime_error("failed to get register event accountaor");
	}

	std::tie(ok, ev.param) = get_member<std::string>(obj, "param");

	return ev;
}

}

std::tuple<bool, Any> parse(Json::Object const& obj)
{
	if (!is_event(obj)) {
		return {false, Event::Any{}};
	}

	auto [ok, class_str] = get_member<std::string>(obj, "class");
	if (!ok) {
		throw std::runtime_error("failed to get event class");
	}

	std::function<Register(Json::Object)> parser;
	std::tie(ok, parser) = select(class_str,
		"register", &parse_register_event
	);

	if (ok) {
		return {true, parser(obj)};
	}

	return {false, {}};
}

}}
