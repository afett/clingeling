/*
   Copyright (c) 2021 Andreas Fett
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
