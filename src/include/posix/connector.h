#pragma once

#include <memory>

namespace Posix {

class Fd;
class SocketAddress;

class Connector {
public:
	virtual void connect(Fd const&, SocketAddress const&) const = 0;
	virtual ~Connector() = default;
};

class ConnectorFactory {
public:
	static std::unique_ptr<ConnectorFactory> create();
	virtual std::unique_ptr<Connector> make_connector() const = 0;
	virtual ~ConnectorFactory() = default;
};

}
