/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>

#include "posix/socket-address.h"
#include "posix/inet-address.h"

bool operator<(sockaddr_storage const& l, sockaddr_storage const& r)
{
	return ::memcmp(&l, &r, sizeof(l)) < 0;
}

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
	case Inet::Address::Family::None:
		throw std::runtime_error("Can't create SocketAddress from unspecified address");
		break;
	}
}

SocketAddress::~SocketAddress() = default;
SocketAddress::SocketAddress() = default;

SocketAddress::SocketAddress(SocketAddress const& o)
:
	size_(o.size_),
	data_(std::make_unique<sockaddr_storage>(*o.data_))
{ }

SocketAddress::SocketAddress(SocketAddress && o)
:
	size_(std::exchange(o.size_, 0)),
	data_(std::move(o.data_))
{ }

SocketAddress & SocketAddress::operator=(SocketAddress const& o)
{
	if (&o != this) {
		size_ = o.size_;
		data_ = std::make_unique<sockaddr_storage>(*o.data_);
	}
	return *this;
}

SocketAddress & SocketAddress::operator=(SocketAddress && o)
{
	if (&o != this) {
		size_ = std::exchange(o.size_, 0);
		data_ = std::move(o.data_);
	}
	return *this;
}

bool operator<(SocketAddress const& l, SocketAddress const& r)
{
	return !(l.size_ == 0 && r.size_ == 0) && std::tie(l.size_, *l.data_) < std::tie(r.size_, *r.data_);
}

std::string to_string(SocketAddress const& addr)
{
	if (addr.size_ == 0) {
		return {};
	}

	auto addrstr = std::string{};
	auto port = uint16_t{0};
	switch (addr.data_->ss_family) {
	case AF_INET:
		addrstr = to_string(Inet::Address{&addr.getSockaddrIn()->sin_addr});
		port = ::ntohs(addr.getSockaddrIn()->sin_port);
		break;
	case AF_INET6:
		addrstr = "[" + to_string(Inet::Address{&addr.getSockaddrIn6()->sin6_addr}) + "]";
		port = ::ntohs(addr.getSockaddrIn6()->sin6_port);
		break;
	default:
		break;
	}
	return addrstr + ':' + std::to_string(port);
}

}
