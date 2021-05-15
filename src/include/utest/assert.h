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

#include <string>

#include "utest/assertion-traits.h"
#include "utest/source-location.h"

namespace UTest {

class AssertionFailure {
public:
	std::string msg;
	SourceLocation loc;
};

template <typename T>
void assert(T const& expr, std::string exprstr, SourceLocation loc)
{
	if (!expr) {
		throw AssertionFailure{std::move(exprstr), std::move(loc)};
	}
}

template <typename L, typename R>
void assert_equal(L const& l, R const& r, std::string expstr, std::string resstr, SourceLocation loc)
{
	if (!EqualTrait<L, R>::equal(l, r)) {
		std::string msg{"Expected: " + expstr + " '" + StringTrait<L>::to_string(l) + "' Result: " + resstr + " '" + StringTrait<R>::to_string(r) + "'"};
		throw AssertionFailure{std::move(msg), std::move(loc)};
	}
}

}
