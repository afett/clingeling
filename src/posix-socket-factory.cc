#include <sys/types.h>
#include <sys/socket.h>

#include "posix/socket-factory.h"
#include "posix/fd.h"
#include "posix/system-error.h"

namespace Posix {

class SocketFactoryImpl : public SocketFactory {
public:
	static std::unique_ptr<SocketFactory> create();
	Fd make_socket(Params const&) const override;
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

Fd SocketFactoryImpl::make_socket(Params const& params) const
{
	auto call_params{socket_params(params)};
	auto res = ::socket(std::get<0>(call_params), std::get<1>(call_params), std::get<2>(call_params));
	if (res == -1) {
		throw make_system_error(errno, Fmt::format("::socket(%s, %s, %s);",
					std::get<0>(call_params), std::get<1>(call_params), std::get<2>(call_params)));
	}
	return Fd{res};
}

}
