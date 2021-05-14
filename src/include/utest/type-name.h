#pragma once

#include <string>
#include <typeinfo>

namespace UTest {

std::string type_name(std::type_info const&);

template <typename T>
std::string type_name()
{
	return type_name(typeid(T));
}

template <typename T>
std::string type_name(T const& t)
{
	return type_name(typeid(t));
}

}
