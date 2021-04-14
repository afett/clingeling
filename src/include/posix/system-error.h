#pragma once

#include <system_error>

#include "fmt.h"

namespace Posix {

inline std::system_error make_system_error(int ev, std::string const& what_arg)
{
	return std::system_error(ev, std::generic_category(), what_arg);
}

template <typename... Args>
std::system_error make_system_error(int ev, std::string const& fmt, Args ...args)
{
	return make_system_error(ev, Fmt::format(fmt, args...));
}

template <typename... Args>
std::system_error make_system_error_detail(int ev, const char* file, size_t line, std::string const& fmt, Args ...args)
{
	return make_system_error(ev, Fmt::format(std::string("%s:%s: ") + fmt, file, line, args...));
}

}

#define POSIX_SYSTEM_ERROR(fmt, ...) Posix::make_system_error_detail(errno, __FILE__, __LINE__, (fmt), __VA_ARGS__)
