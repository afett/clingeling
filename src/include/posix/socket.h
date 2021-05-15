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

class StreamSocket : public Posix::Socket {
public:
	enum class State {
		init,
		in_progress,
		connected,
		error,
	};

	virtual State state() const = 0;
	virtual void connect_continue() = 0;
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
		Fd::Options options = {};
	};

	static std::unique_ptr<SocketFactory> create();
	virtual std::shared_ptr<Socket> make_socket(Params const&) const = 0;
	virtual std::shared_ptr<StreamSocket> make_stream_socket(Params const&) const = 0;
	virtual ~SocketFactory() = default;
};

}
