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
