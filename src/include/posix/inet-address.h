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
#include <tuple>

struct in_addr;
struct in6_addr;

namespace Posix {
namespace Inet {

class Address {
public:
	enum class Family {
		None,
		Inet,
		Inet6,
	};

	Address();
	explicit Address(std::string const& str)
	:
		Address(str.c_str())
	{ }

	explicit Address(const char *);
	explicit Address(const in_addr *);
	explicit Address(const in6_addr *);

	explicit operator bool() const
	{
		return family_ != Family::None;
	}

	size_t size() const
	{
		switch (family_) {
		case Family::None: return 0;
		case Family::Inet: return 4;
		case Family::Inet6: return 16;
		}
		return size_t(-1);
	}

	const uint8_t * data() const
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

	friend bool operator<(Address const& l, Address const& r)
	{
		return std::tie(l.family_, l.data_) < std::tie(r.family_, r.data_);
	}

private:
	Family family_ = Family::None;
	static constexpr size_t data_size = 16;
	std::array<uint8_t, data_size> data_ = {0};
};

inline bool operator>(Address const& l, Address const& r)
{
	return r < l;
}

inline bool operator==(Address const& l, Address const& r)
{
	return !(l < r) && !(l > r);
}

inline bool operator!=(Address const& l, Address const& r)
{
	return !(l == r);
}

inline bool operator<=(Address const& l, Address const& r)
{
	return (l < r) || (l == r);
}

inline bool operator>=(Address const& l, Address const& r)
{
	return (l > r) || (l == r);
}

std::string to_string(Address const&);

}}
