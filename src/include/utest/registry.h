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

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "utest/source-location.h"
#include "utest/type-name.h"

namespace UTest {

class Registry {
public:
	static Registry & get();

	void register_test(std::string const&, SourceLocation const&, std::function<void(void)> const&);

	struct Test {
		std::string name;
		SourceLocation loc;
		std::function<void(void)> run;
	};

	using const_iterator = std::vector<Test>::const_iterator;

	const_iterator begin() const;
	const_iterator end() const;

private:
	std::vector<Test> tests_;
};

template <typename T>
class Registrator {
public:
	Registrator(SourceLocation loc)
	{
		Registry::get().register_test(type_name<T>(), loc, []() {
			auto test = std::make_unique<T>();
			(*test)();
		});
	}
};

}
