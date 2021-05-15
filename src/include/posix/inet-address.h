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
#include <array>

struct in_addr;
struct in6_addr;

namespace Posix {
namespace Inet {

class Address {
public:
	enum class Family {
		Inet,
		Inet6,
	};

	explicit Address(std::string const& str)
	:
		Address(str.c_str())
	{ }

	explicit Address(const char *);

	size_t size() const
	{
		return family_ == Family::Inet ? 4 : 16;
	}

	const char * data() const
	{
		return data_.data();
	}

	const in_addr * in_addr_data() const
	{
		return reinterpret_cast<in_addr const*>(data_.data());
	}

	const in6_addr * in6_addr_data() const
	{
		return reinterpret_cast<in6_addr const*>(data_.data());
	}

	Family family() const
	{
		return family_;
	}

private:
	Family family_ = Family::Inet;
	std::array<char, 16> data_ = {};
};

}
}
