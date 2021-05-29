/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include <cassert>
#include <cstring>

#include <arpa/inet.h>

#include "posix/inet-address.h"
#include "posix/system-error.h"

namespace Posix {
namespace Inet {

Address::Address() = default;

Address::Address(const char *str)
{
	for (auto family : {std::make_pair(AF_INET, Family::Inet), std::make_pair(AF_INET6, Family::Inet6)}) {
		switch (::inet_pton(family.first, str, data_.data())) {
		case 1:
			family_ = family.second;
			return;
		case 0:
			continue;
		case -1:
			throw POSIX_SYSTEM_ERROR("::inet_pton(%s, %s, %x);", family.first, str, data_.data());
		default:
			break;
		}
	}

	throw POSIX_SYSTEM_ERROR("::inet_pton(%s);", str);
	assert(false);
}

Address::Address(const in_addr * addr)
:
	family_(Family::Inet),
	data_()
{
	static_assert(sizeof(in_addr) <= data_size);
	::memcpy(data_.data(), addr, sizeof(in_addr));
}

Address::Address(const in6_addr * addr)
:
	family_(Family::Inet6),
	data_()
{
	static_assert(sizeof(in6_addr) <= data_size);
	::memcpy(data_.data(), addr, sizeof(in6_addr));
}

namespace {

std::string to_string(Address::Family family)
{
	switch (family) {
	case Address::Family::None: return "<undefined>";
	case Address::Family::Inet: return "AF_INET";
	case Address::Family::Inet6: return "AF_INET6";
	}
	return "";
}

}

std::string to_string(Address const& addr)
{
	char buf[INET6_ADDRSTRLEN];
	const char* res{nullptr};

	switch (addr.family()) {
	case Address::Family::None:
		return "";
	case Address::Family::Inet:
		res = ::inet_ntop(AF_INET, addr.in_addr_data(), buf, sizeof(buf));
		break;
	case Address::Family::Inet6:
		res = ::inet_ntop(AF_INET6, addr.in_addr_data(), buf, sizeof(buf));
		break;
	}
	if (res == nullptr) {
		throw POSIX_SYSTEM_ERROR("::inet_pton(%s, %x, %x, %s);", to_string(addr.family()), addr.in_addr_data(), &buf, sizeof(buf));
	}
	return buf;
}

}
}
