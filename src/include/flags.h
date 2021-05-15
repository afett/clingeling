#pragma once

#include <type_traits>

template <typename T>
class Flags {
public:
	using Type = T;

	Flags() = default;
	Flags(Flags const&) = default;
	Flags & operator=(Flags const&) = default;

	Flags(Type type)
	:
		value_(1 << static_cast<decltype(value_)>(type))
	{ }

	explicit operator bool() const
	{
		return value_ != 0;
	}

	Flags & operator|=(Flags const& o)
	{
		value_ |= o.value_;
		return *this;
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

	friend Flags operator|(Flags const& l, Flags const& r)
	{
		return Flags(l) |= r;
	}

	friend Flags operator&(Flags const& l, Flags const& r)
	{
		return Flags(l) &= r;
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
	std::underlying_type_t<Type> value_ = 0;
};
