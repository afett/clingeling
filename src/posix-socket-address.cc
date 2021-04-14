#include <sys/socket.h>
#include <arpa/inet.h>

#include "posix/socket-address.h"
#include "posix/inet-address.h"

namespace Posix {

namespace {


}

SocketAddress::SocketAddress(Inet::Address const&, uint16_t)
:
	data_{std::make_unique<sockaddr_storage>()}
{
	switch (addr.family()) {
	case Inet::Address::Family::Inet:
		::init_sockaddr_inet(::htons(port));
		break;
	case Inet::Address::Family::Inet6:
		::init_sockaddr_inet6(::htons(port));
		break;
	}
}

SocketAddress::~SocketAddress() = default;

}
