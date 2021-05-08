#pragma once

#include <json/types.h>

namespace Json {

std::string to_string(Value const&);
std::string to_string(Object const&);
std::string to_string(Array const&);

}
