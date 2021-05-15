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

#include <exception>

#include "utest/assert.h"
#include "utest/registry.h"

#define UTEST_ASSERT(expr) \
	UTest::assert((expr), std::string{ #expr }, {__FILE__, __LINE__})

#define UTEST_ASSERT_EQUAL(expected, result) \
	UTest::assert_equal((expected), (result), std::string{ #expected }, std::string{ #result }, {__FILE__, __LINE__})

#define UTEST_ASSERT_THROW(expr, exception) \
	try {   auto utest_failure_location = UTest::SourceLocation{__FILE__, __LINE__};         \
		(expr);                                                     \
		throw UTest::AssertionFailure{std::string("Expected ") + #expr + " to throw exception of type " + #exception, utest_failure_location}; \
	} catch (exception const& e) {                                      \
	}

#define UTEST_CASE(name)                                        \
class name {                                                   \
public:                                                        \
	void operator()();                                     \
};                                                             \
auto name ## Registrator = UTest::Registrator<name>{{__FILE__, __LINE__}}; \
void name::operator()()

#define UTEST_CASE_WITH_FIXTURE(name, fixture)                  \
class name : public fixture {                                  \
public:                                                        \
	void operator()();                                     \
};                                                             \
auto name ## Registrator = UTest::Registrator<name>{{__FILE__, __LINE__}};  \
void name::operator()()