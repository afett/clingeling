/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include <sys/types.h>
#include <sys/socket.h>

#include "posix/socket.h"
#include "posix/socket-address.h"
#include "posix/fd.h"
#include "posix/system-error.h"

namespace Posix {

class SocketImpl : public Socket {
public:
	explicit SocketImpl(std::shared_ptr<Fd> const&);

	void bind(SocketAddress const&) const override;
	void connect(SocketAddress const&) const override;
	std::error_code get_socket_error() const override;

	int get() const override
	{
		return fd_->get();
	}

	size_t write(void const* buf, size_t count) const override
	{
		return fd_->write(buf, count);
	}

	size_t read(void * buf, size_t count) const override
	{
		return fd_->read(buf, count);
	}

private:
	int getsockopt(int level, int optname) const;

	std::shared_ptr<Fd> fd_;
};

SocketImpl::SocketImpl(std::shared_ptr<Fd> const& fd)
:
	fd_(fd)
{ }

void SocketImpl::bind(SocketAddress const& addr) const
{
	auto res = ::bind(fd_->get(), addr.getSockaddr(), addr.size());
	if (res == -1) {
		throw POSIX_SYSTEM_ERROR("::bind(fd.get(), addr.data(), addr.size());", fd_.get(), addr.getSockaddr(), addr.size());
	}
}

void SocketImpl::connect(SocketAddress const& addr) const
{
	int res{-1};
	do {
		res = ::connect(fd_->get(), addr.getSockaddr(), addr.size());
	} while (res == -1 && errno == EINTR);

	if (res == -1) {
		throw POSIX_SYSTEM_ERROR("::connect(%s, %x, %s);", fd_.get(), addr.getSockaddr(), addr.size());
	}
}

int SocketImpl::getsockopt(int level, int optname) const
{
	int value{0};
	socklen_t len{sizeof(value)};
	auto res = ::getsockopt(fd_->get(), level, optname, &value, &len);
	if (res == -1) {
		throw POSIX_SYSTEM_ERROR("::getsockopt(%s, %s, %s, %x, %s);:", fd_.get(), level, optname, &value, sizeof(value));
	}
	return value;
}

std::error_code SocketImpl::get_socket_error() const
{
	return {getsockopt(SOL_SOCKET, SO_ERROR), std::generic_category()};
}

class StreamSocketImpl : public StreamSocket {
public:
	explicit StreamSocketImpl(std::shared_ptr<Socket> const& socket)
	:
		socket_(socket)
	{ }

	State state() const override
	{
		return state_;
	}

	void bind(Posix::SocketAddress const& addr) const override
	{
		socket_->bind(addr);
	}

	int get() const override
	{
		return socket_->get();
	}

	size_t write(void const* buf, size_t count) const override
	{
		return socket_->write(buf, count);
	}

	size_t read(void *buf, size_t count) const override
	{
		return socket_->read(buf, count);
	}

	std::error_code get_socket_error() const override
	{
		return socket_->get_socket_error();
	}

	void connect(SocketAddress const& addr) const override
	{
		try {
			socket_->connect(addr);
		} catch (std::system_error const& e) {
			if (e.code() == std::errc::operation_in_progress) {
				state_ = State::in_progress;
				return;
			} else {
				state_ = State::error;
				throw_backtrace();
			}
		}
		state_ = State::connected;
	}

	void connect_continue()
	{
		if (state_ != State::in_progress) {
			throw std::runtime_error("Socket connect not in progress");
		}

		auto ec{socket_->get_socket_error()};
		if (ec) {
			throw std::system_error(ec);
		}

		state_ = State::connected;
	}

private:
	mutable State state_ = State::init;
	std::shared_ptr<Socket> socket_;
};

class SocketFactoryImpl : public SocketFactory {
public:
	static std::unique_ptr<SocketFactory> create();
	std::shared_ptr<Socket> make_socket(Params const&) const override;
	std::shared_ptr<StreamSocket> make_stream_socket(Params const&) const override;
};

std::unique_ptr<SocketFactory> SocketFactory::create()
{
	return std::make_unique<SocketFactoryImpl>();
}

namespace {

std::tuple<int, int, int> socket_params(SocketFactory::Params const& params)
{
	std::tuple<int, int, int> res{0, 0, 0};
	switch (params.domain) {
	case SocketFactory::Params::Domain::Unix:
		std::get<0>(res) = AF_UNIX;
		break;
	case SocketFactory::Params::Domain::Inet:
		std::get<0>(res) = AF_INET;
		break;
	case SocketFactory::Params::Domain::Inet6:
		std::get<0>(res) = AF_INET6;
		break;
	}

	switch (params.type) {
	case SocketFactory::Params::Type::Stream:
		std::get<1>(res) = SOCK_STREAM;
		break;
	case SocketFactory::Params::Type::Dgram:
		std::get<1>(res) = SOCK_DGRAM;
		break;
	}

	if (params.options & Fd::Option::nonblock) {
		std::get<1>(res) |= SOCK_NONBLOCK;
	}

	if (params.options & Fd::Option::cloexec) {
		std::get<1>(res) |= SOCK_CLOEXEC;
	}

	return res;
}

}

std::shared_ptr<Socket> SocketFactoryImpl::make_socket(Params const& params) const
{
	auto call_params{socket_params(params)};
	auto fd = ::socket(std::get<0>(call_params), std::get<1>(call_params), std::get<2>(call_params));
	if (fd == -1) {
		throw make_system_error(errno, Fmt::format("::socket(%s, %s, %s);",
					std::get<0>(call_params), std::get<1>(call_params), std::get<2>(call_params)));
	}
	return std::make_shared<SocketImpl>(Fd::create(fd));
}

std::shared_ptr<StreamSocket> SocketFactoryImpl::make_stream_socket(Params const& params) const
{
	auto stream_params = params;
	stream_params.type = Params::Type::Stream;
	return std::make_shared<StreamSocketImpl>(make_socket(stream_params));
}

}
