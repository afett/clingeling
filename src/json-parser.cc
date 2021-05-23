/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include "json/parser.h"

#include <istream>

namespace {

void parse_whitespace(std::istream & in)
{
	while (::isspace(in.peek())) {
		in.get();
	}
}

std::string parse_string(std::istream & in)
{
	if (in.get() != '"') {
		throw std::runtime_error("parse_string: expected: '\"'");
	}

	std::string res;
	while (in.peek() != '"') {
		res += in.get();
	}
	in.get();

	return res;
}

template <typename T>
T parse_bareword(std::istream & in, std::string const& word, T value)
{
	std::string res;
	while (res.size() != word.size()) {
		res += in.get();
	}
	if (res != word) {
		throw std::runtime_error(std::string("parse_bareword:") + "Expected: " + word + " got " + res);
	}
	return value;
}

int64_t parse_number(std::istream & in)
{
	int64_t res{0};
	in >> res;
	return res;
}

Json::Value parse_value(std::istream & in, size_t depth)
{
	if (++depth > 256) {
		throw std::runtime_error("parse_value: nesting depth exceeded");
	}

	parse_whitespace(in);
	switch (in.peek()) {
	case '{': return Json::Value{Json::parse_object(in, depth)};
	case '[': return Json::Value{Json::parse_array(in, depth)};
	case '"': return Json::Value{parse_string(in)};
	case 't': return Json::Value{parse_bareword(in, "true", true)};
	case 'f': return Json::Value{parse_bareword(in, "false", false)};
	case 'n': return Json::Value{parse_bareword(in, "null", nullptr)};
	case '+': case '-': case '0'...'9':
		  return Json::Value{parse_number(in)};
	default:
		  throw std::runtime_error("parse_value: invalid char");
	}

	return {};
}

std::pair<std::string, Json::Value> parse_member(std::istream & in, size_t depth)
{
	auto key{parse_string(in)};
	parse_whitespace(in);
	if (in.get() != ':') {
		throw std::runtime_error("parse_object: parse_member: expected: ':'");
	}
	parse_whitespace(in);
	return std::make_pair(key, parse_value(in, depth));
}

bool parse_comma(std::istream & in)
{
	parse_whitespace(in);
	if (in.peek() == ',') {
		in.get();
		return true;
	}
	return false;
}

}

namespace Json {

Object parse_object(std::istream & in, size_t depth)
{
	parse_whitespace(in);
	if (in.get() != '{') {
		throw std::runtime_error("parse_object: expected: '{'");
	}

	Object res{};
	parse_whitespace(in);
	if (in.peek() == '}') {
		in.get();
		return {};
	}

	do {
		res.insert(parse_member(in, depth));
	} while (parse_comma(in));

	if (in.get() != '}') {
		throw std::runtime_error("parse_object: expected: '}' or ','");
	}

	return res;
}

Array parse_array(std::istream & in, size_t depth)
{
	parse_whitespace(in);
	if (in.get() != '[') {
		throw std::runtime_error("parse_array: expected: '['");
	}

	Array res{};
	parse_whitespace(in);
	if (in.peek() == ']') {
		in.get();
		return {};
	}

	do {
		res.push_back(parse_value(in, depth));
	} while (parse_comma(in));

	if (in.get() != ']') {
		throw std::runtime_error("parse_array: expected: ']'");
	}

	return res;
}

Value parse_document(std::istream & in, size_t depth)
{
	parse_whitespace(in);
	switch (in.peek()) {
	case '{': return Value{parse_object(in, depth)};
	case '[': return Value{parse_array(in, depth)};
	default:
		  throw std::runtime_error("parse_document: must be Object or Array");
	}

	return {};
}

}
