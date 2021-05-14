#pragma once

#include <sstream>

namespace UTest {

template <typename T>
class StringTrait {
public:
	static std::string to_string(T const& t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
};

template <typename L, typename R = L>
class EqualTrait {
public:
	static bool equal(L const& l, R const& r)
	{
		return l == r;
	}
};

}
