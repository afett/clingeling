#pragma once

#include <cppunit/TestAssert.h>

namespace CppUnit {

template <>
struct assertion_traits<nullptr_t>
{
	static bool equal(const nullptr_t& x, const nullptr_t& y)
	{
		return x == y;
	}

	static bool less(const nullptr_t& x, const nullptr_t& y)
	{
		return x < y;
	}

	static bool lessEqual(const nullptr_t& x, const nullptr_t& y)
	{
		return x <= y;
	}

	static std::string toString(const nullptr_t&)
	{
		return "nullptr";
	}
};

}
