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
	case 't': throw std::runtime_error("parse_value: true unimplemented");
	case 'f': throw std::runtime_error("parse_value: false unimplemented");
	case 'n': throw std::runtime_error("parse_value: null unimplemented");
	case '+': case '-': case '1'...'9':
		  throw std::runtime_error("parse_value: number unimplemented");
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
		throw std::runtime_error("parse_object: expected: '}'");
	}

	return {};
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

	return {};
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
