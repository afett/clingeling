#pragma once

#include <cstdint>

namespace EPoll {

class Event {
public:
	enum class Type {
		In = 0,
		Out,
		RdHup,
		Pri,
		Err,
		Hup,
	};

	Event() = default;

	Event(Type type)
	:
		value_(1 << static_cast<decltype(value_)>(type))
	{ }

	explicit operator bool() const
	{
		return value_ != 0;
	}

	Event & operator|=(Event const& o)
	{
		value_ |= o.value_;
		return *this;
	}

	Event & operator&=(Event const& o)
	{
		value_ &= o.value_;
		return *this;
	}

	friend Event operator~(Event o)
	{
		o.value_ = ~o.value_;
		return o;
	}

	friend Event operator|(Event const& l, Event const& r)
	{
		return Event(l) |= r;
	}

	friend Event operator&(Event const& l, Event const& r)
	{
		return Event(l) &= r;
	}

	friend bool operator==(Event const& l, Event const& r)
	{
		return l.value_ == r.value_;
	}

	friend bool operator!=(Event const& l, Event const& r)
	{
		return l.value_ != r.value_;
	}

private:
	uint32_t value_ = 0;
};

}
