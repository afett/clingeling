/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include "netlink/socket.h"
#include "posix/socket-address.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>

namespace Netlink {

class SocketImpl : public Socket {
public:
	explicit SocketImpl(std::shared_ptr<Posix::Socket> const&);

	std::error_code get_socket_error() const override
	{
		return socket_->get_socket_error();
	}

	int get() const override
	{
		return socket_->get();
	}

	size_t write(void const* buf, size_t count) const override
	{
		return socket_->write(buf, count);
	}

	size_t read(void * buf, size_t count) const override
	{
		return socket_->read(buf, count);
	}

	void bind(Posix::SocketAddress const& addr) const override
	{
		socket_->bind(addr);
	}

	void bind(std::uint32_t) const override;

	void connect(Posix::SocketAddress const& addr) const override
	{
		socket_->connect(addr);
	}

private:
	std::shared_ptr<Posix::Socket> socket_;
};

SocketImpl::SocketImpl(std::shared_ptr<Posix::Socket> const& socket)
:
	socket_(socket)
{ }

void SocketImpl::bind(std::uint32_t groups) const
{
	auto addr_storage = std::make_unique<sockaddr_storage>();
	auto * addr = reinterpret_cast<sockaddr_nl*>(addr_storage.get());
	addr->nl_family = AF_NETLINK;
	addr->nl_pid = 0;
	addr->nl_groups = groups;

	bind(Posix::SocketAddress(std::move(addr_storage), sizeof(sockaddr_nl)));
}

class SocketFactoryImpl : public SocketFactory {
public:
	SocketFactoryImpl();

	std::shared_ptr<Socket> make_socket(Family, Posix::Fd::Options const&) const override;

private:
	std::unique_ptr<Posix::SocketFactory> socket_factory_;
};

SocketFactoryImpl::SocketFactoryImpl()
:
	socket_factory_{Posix::SocketFactory::create()}
{ }

std::unique_ptr<SocketFactory> SocketFactory::create()
{
	return std::make_unique<SocketFactoryImpl>();
}

std::shared_ptr<Socket> SocketFactoryImpl::make_socket(Family family, Posix::Fd::Options const& options) const
{
	int netlink_family{0};
	switch (family) {
	case Family::Generic:
		netlink_family = NETLINK_GENERIC;
		break;
	case Family::KObjectUEvent:
		netlink_family = NETLINK_KOBJECT_UEVENT;
		break;
	}
	return std::make_shared<SocketImpl>(socket_factory_->make_socket({AF_NETLINK, SOCK_DGRAM, netlink_family}, options));
}

}

