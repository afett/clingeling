#pragma once

#include <memory>

namespace Posix {

class Fd;

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
	virtual Fd make_socket(Params const&) const = 0;
	virtual ~SocketFactory() = default;
};

}
