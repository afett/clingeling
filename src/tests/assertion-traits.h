#pragma once

#include "utest/assertion-traits.h"

namespace UTest {

template <>
struct StringTrait<nullptr_t>
{
	static std::string to_string(const nullptr_t&)
	{
		return "nullptr";
	}
};

}
