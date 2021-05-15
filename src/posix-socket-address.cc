/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include <sys/socket.h>
#include <arpa/inet.h>

#include "posix/socket-address.h"
#include "posix/inet-address.h"

namespace {

size_t init_sockaddr_inet(sockaddr_in *sockaddr, uint16_t port, in_addr const* addr)
{
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = port;
	sockaddr->sin_addr = *addr;
	return sizeof(sockaddr_in);
}

size_t init_sockaddr_inet6(sockaddr_in6 *sockaddr, uint16_t port, in6_addr const* addr)
{
	sockaddr->sin6_family = AF_INET6;
	sockaddr->sin6_port = port;
	sockaddr->sin6_addr = *addr;
	return sizeof(sockaddr_in6);
}

}

namespace Posix {

SocketAddress::SocketAddress(Inet::Address const& addr, uint16_t port)
:
	data_{std::make_unique<sockaddr_storage>()}
{
	switch (addr.family()) {
	case Inet::Address::Family::Inet:
		size_ = ::init_sockaddr_inet(reinterpret_cast<sockaddr_in*>(data_.get()), ::htons(port), addr.in_addr_data());
		break;
	case Inet::Address::Family::Inet6:
		size_ = ::init_sockaddr_inet6(reinterpret_cast<sockaddr_in6*>(data_.get()), ::htons(port), addr.in6_addr_data());
		break;
	}
}

SocketAddress::~SocketAddress() = default;

}
