#include <sys/types.h>
#include <sys/socket.h>

#include "posix/socket.h"
#include "posix/socket-address.h"
#include "posix/fd.h"
#include "posix/system-error.h"

namespace Posix {

class SocketImpl : public Socket {
public:
	explicit SocketImpl(Fd const&);

	Fd get_fd() const override;
	void bind(SocketAddress const&) const override;
	void connect(SocketAddress const&) const override;
	std::error_code get_socket_error() const override;
private:
	int getsockopt(int level, int optname) const;

	Fd fd_;
};

SocketImpl::SocketImpl(Fd const& fd)
:
	fd_(fd)
{ }

Fd SocketImpl::get_fd() const
{
	return fd_;
}

void SocketImpl::bind(SocketAddress const& addr) const
{
	auto res = ::bind(fd_.get(), addr.getSockaddr(), addr.size());
	if (res == -1) {
		throw POSIX_SYSTEM_ERROR("::bind(fd.get(), addr.data(), addr.size());", fd_.get(), addr.getSockaddr(), addr.size());
	}
}

void SocketImpl::connect(SocketAddress const& addr) const
{
	int res{-1};
	do {
		res = ::connect(fd_.get(), addr.getSockaddr(), addr.size());
	} while (res == -1 && errno == EINTR);

	if (res == -1) {
		throw POSIX_SYSTEM_ERROR("::connect(%s, %x, %s);", fd_.get(), addr.getSockaddr(), addr.size());
	}
}

int SocketImpl::getsockopt(int level, int optname) const
{
	int value{0};
	socklen_t len{sizeof(value)};
	auto res = ::getsockopt(fd_.get(), level, optname, &value, &len);
	if (res == -1) {
		throw POSIX_SYSTEM_ERROR("::getsockopt(%s, %s, %s, %x, %s);:", fd_.get(), level, optname, &value, sizeof(value));
	}
	return value;
}

std::error_code SocketImpl::get_socket_error() const
{
	return {getsockopt(SOL_SOCKET, SO_ERROR), std::generic_category()};
}

class SocketFactoryImpl : public SocketFactory {
public:
	static std::unique_ptr<SocketFactory> create();
	std::shared_ptr<Socket> make_socket(Params const&) const override;
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

	if (params.nonblock) {
		std::get<1>(res) |= SOCK_NONBLOCK;
	}

	if (params.cloexec) {
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
	return std::make_shared<SocketImpl>(Fd{fd});
}

}
