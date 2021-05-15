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

#include <sstream>
#include <functional>

namespace Fmt {

template <typename... Args>
std::string format(std::string const& fmt_str, Args ...args)
{
	auto stringer = std::initializer_list<std::function<void(std::ostream &, int)>>{
		[&args]() { return [&args](std::ostream & os, int c) {
			switch (c) {
			case 'x':
				os << "0x" << std::hex << args << std::dec;
				break;
			default:
				os << args;
			}
		}; }()...
	};

	auto do_fmt{false};
	auto to_string(std::begin(stringer));
	std::stringstream ss;
	for (auto c : fmt_str) {
		if (to_string == std::end(stringer)) {
			ss << c;
			continue;
		}

		if (c == '%') {
			if (std::exchange(do_fmt, !do_fmt)) {
				ss << c;
			}
			continue;
		}

		if (std::exchange(do_fmt, false)) {
			(*to_string++)(ss, c);
			continue;
		}

		ss << c;
	}

	return ss.str();
}

}
