#pragma once

#include "json/types.h"

namespace Json {

Object parse_object(std::istream &, size_t depth = 0);
Array parse_array(std::istream &, size_t depth = 0);
Value parse_document(std::istream &, size_t depth = 0);

}
