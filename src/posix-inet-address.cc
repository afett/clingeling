#include <cassert>

#include <arpa/inet.h>

#include "posix/inet-address.h"
#include "posix/system-error.h"

namespace Posix {
namespace Inet {

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

	assert(false);
}

}
}
