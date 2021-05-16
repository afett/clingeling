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

#include <type_traits>
#include <initializer_list>
#include <functional>

template <typename T> struct IsFlagType;

#define DECLARE_FLAG_TYPE(enum_type) \
template <> struct IsFlagType<enum_type> { static constexpr bool value = true; }

template <typename T>
class Flags {
public:
	using Flag = T;

	static_assert(IsFlagType<Flag>::value);

	Flags() = default;
	Flags(Flags const&) = default;
	Flags & operator=(Flags const&) = default;

	explicit Flags(Flag flag)
	:
		value_(1 << static_cast<decltype(value_)>(flag))
	{ }

	explicit operator bool() const
	{
		return value_ != 0;
	}

	Flags & operator=(Flag const& o)
	{
		return *this = Flags(o);
	}

	Flags & operator|=(Flags const& o)
	{
		value_ |= o.value_;
		return *this;
	}

	Flags & operator|=(Flag const& o)
	{
		return *this |= Flags(o);
	}

	Flags & operator&=(Flags const& o)
	{
		value_ &= o.value_;
		return *this;
	}

	friend Flags operator~(Flags o)
	{
		o.value_ = ~o.value_;
		return o;
	}

	friend bool operator==(Flags const& l, Flags const& r)
	{
		return l.value_ == r.value_;
	}

	friend bool operator!=(Flags const& l, Flags const& r)
	{
		return l.value_ != r.value_;
	}

private:
	std::underlying_type_t<Flag> value_ = 0;
};

template <typename T>
typename std::enable_if_t<IsFlagType<T>::value, Flags<T>>
operator|(T const& l, T const& r)
{
	return Flags<T>(l) |= r;
}

template <typename T>
bool operator!=(Flags<T> const& l, T const& r)
{
	return l != Flags(r);
}

template <typename T>
Flags<T> operator&(Flags<T> const& l, T const& r)
{
	return l & Flags(r);
}

template <typename T>
Flags<T> operator|(Flags<T> const& l, Flags<T> const& r)
{
	return Flags<T>(l) |= r;
}

template <typename T>
Flags<T> operator&(Flags<T> const& l, Flags<T> const& r)
{
	return Flags<T>(l) &= r;
}

template <typename T>
void dispatch(
	Flags<T> const& flags,
	std::initializer_list<std::tuple<T, std::function<void(void)>>> actions)
{
	for (auto const& action : actions) {
		if (flags & std::get<0>(action)) {
			std::get<1>(action)();
		}
	}
}
