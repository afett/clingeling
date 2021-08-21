/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/
#include "baresip-proto-parser.h"
#include "baresip/command.h"

#include <functional>
#include <stdexcept>

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

bool is_response(Json::Object const& obj)
{
	auto is_resp = get_member<bool>(obj, "response");
	return std::get<0>(is_resp) && std::get<1>(is_resp);
}

}

namespace Baresip {
namespace Event {

namespace {

std::tuple<bool, Any> parse_register_event(Json::Object const& obj)
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
		return {false, {}};
	}

	std::tie(ok, ev.accountaor) = get_member<std::string>(obj, "accountaor");
	if (!ok) {
		throw std::runtime_error("failed to get register event accountaor");
	}

	std::tie(std::ignore, ev.param) = get_member<std::string>(obj, "param");

	return {true, ev};
}

std::tuple<bool, Any> parse_call_event(Json::Object const& obj)
{
	Event::Call ev;
	auto [ok, str] = get_member<std::string>(obj, "type");
	if (!ok) {
		throw std::runtime_error("failed to get call event type");
	}
	std::tie(ok, ev.type) = select(str,
		"CALL_CLOSED", Call::Type::Closed,
		"CALL_ESTABLISHED", Call::Type::Established,
		"CALL_INCOMING", Call::Type::Incoming,
		"CALL_RINGING", Call::Type::Ringing
	);
	if (!ok) {
		return {false, {}};
	}

	std::tie(ok, ev.accountaor) = get_member<std::string>(obj, "accountaor");
	if (!ok) {
		throw std::runtime_error("failed to get call event accountaor");
	}

	std::tie(ok, str) = get_member<std::string>(obj, "direction");
	if (!ok) {
		throw std::runtime_error("failed to get call event driection");
	}
	std::tie(ok, ev.direction) = select(str,
		"incoming", Call::Direction::Incoming,
		"outgoing", Call::Direction::Outgoing
	);
	if (!ok) {
		throw std::runtime_error("failed to translate call event direction");
	}

	std::tie(ok, ev.peeruri) = get_member<std::string>(obj, "peeruri");
	if (!ok) {
		throw std::runtime_error("failed to get call event peeruri");
	}

	std::tie(ok, ev.id) = get_member<std::string>(obj, "id");
	if (!ok) {
		throw std::runtime_error("failed to get call event id");
	}

	std::tie(std::ignore, ev.param) = get_member<std::string>(obj, "param");

	return {true, ev};
}

}

std::tuple<bool, Any> parse(Json::Object const& obj)
{
	if (!is_event(obj)) {
		return {false, {}};
	}

	auto [ok, class_str] = get_member<std::string>(obj, "class");
	if (!ok) {
		throw std::runtime_error("failed to get event class");
	}

	decltype(&parse_register_event) parser;
	std::tie(ok, parser) = select(class_str,
		"register", &parse_register_event,
		"call", &parse_call_event
	);
	if (!ok) {
		return {false, {}};
	}

	return parser(obj);
}

} // Event

namespace Command {

std::tuple<bool, Response> parse(Json::Object const& obj)
{
	if (!is_response(obj)) {
		return {false, {}};
	}

	Response resp;
	auto ok{false};
	std::tie(ok, resp.ok) = get_member<bool>(obj, "ok");
	if (!ok) {
		throw std::runtime_error("failed to get response ok state");
	}

	std::tie(ok, resp.data) = get_member<std::string>(obj, "data");
	if (!ok) {
		throw std::runtime_error("failed to get response data");
	}

	std::tie(std::ignore, resp.token) = get_member<std::string>(obj, "token");

	return {true, resp};
}

}}
