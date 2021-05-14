#include <memory>

#include <cxxabi.h>

#include "utest/type-name.h"

namespace UTest {

std::string type_name(std::type_info const& ti)
{
	int status{0};
	auto name = std::unique_ptr<char, decltype(&::free)>{abi::__cxa_demangle(ti.name(), nullptr, nullptr, &status), &::free};
	if (status == -1) {
		throw std::bad_alloc();
	}
	if (status != 0) {
		throw std::invalid_argument("abi::__cxa_demangle(ti.name()");
	}
	return name.get();
}

}
