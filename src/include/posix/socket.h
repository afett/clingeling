#pragma once

#include <memory>
#include <system_error>

#include <posix/fd.h>

namespace Posix {

class SocketAddress;

class Socket : public Fd {
public:
	virtual void bind(SocketAddress const&) const = 0;
	virtual void connect(SocketAddress const&) const = 0;
	virtual std::error_code get_socket_error() const = 0;

	virtual ~Socket() = default;
};

class SocketFactory {
public:
	struct Params {
		enum class Domain {
			Unix,
			Inet,
			Inet6
		};

		enum class Type {
			Stream,
			Dgram,
		};

		Domain domain = Domain::Unix;
		Type type = Type::Stream;
		bool nonblock = false;
		bool cloexec = false;
	};

	static std::unique_ptr<SocketFactory> create();
	virtual std::shared_ptr<Socket> make_socket(Params const&) const = 0;
	virtual ~SocketFactory() = default;
};

}
