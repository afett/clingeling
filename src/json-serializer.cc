#include "json/serializer.h"

#include <sstream>

namespace {

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<std::remove_reference_t<Ts>...>;

std::string quote(std::string str)
{
	return str.insert(0, "\"") + '"';
}

}

namespace Json {

std::string to_string(Value const& v)
{
	std::stringstream os;
	std::visit(::overloaded{
		[&os](nullptr_t)            { os << "null"; },
		[&os](bool b)               { os << (b ? "true" : "false"); },
		[&os](uint64_t ui)          { os << ui; },
		[&os](int64_t i)            { os << i; },
		[&os](std::string const& s) { os << quote(s); },
		[&os](Object const& o)      { os << to_string(o); },
		[&os](Array const& a)       { os << to_string(a); },
			}, v.get());
	return os.str();
}

std::string to_string(Object const& o)
{
	std::stringstream os;
	std::string sep;
	for (auto const& m : o) {
		os << sep << quote(m.first) << ": " << to_string(m.second);
		sep = ", ";
	}
	return '{' + os.str() + '}';
}

std::string to_string(Array const& a)
{
	std::stringstream os;
	std::string sep;
	for (auto const& e : a) {
		os << sep << to_string(e);
		sep = ", ";
	}
	return '[' + os.str() + ']';
}

}
