#pragma once

#include <type_traits>

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
