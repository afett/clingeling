#include <sys/socket.h>

#include "posix/connector.h"
#include "posix/fd.h"
#include "posix/socket-address.h"
#include "posix/system-error.h"

namespace Posix {

class ConnectorImpl : public Connector {
public:
	void connect(Fd const&, SocketAddress const&) const override;
};

void ConnectorImpl::connect(Fd const& fd, SocketAddress const& addr) const
{
	int res{-1};
	do {
		res = ::connect(fd.get(), addr.getSockaddr(), addr.size());
	} while (res == -1 && errno == EINTR);

	if (res == -1) {
		throw POSIX_SYSTEM_ERROR("::connect(%s, %x, %s);", fd.get(), addr.getSockaddr(), addr.size());
	}
}

class ConnectorFactoryImpl : public ConnectorFactory {
public:
	std::unique_ptr<Connector> make_connector() const override;
};

std::unique_ptr<ConnectorFactory> ConnectorFactory::create()
{
	return std::make_unique<ConnectorFactoryImpl>();
}

std::unique_ptr<Connector> ConnectorFactoryImpl::make_connector() const
{
	return std::make_unique<ConnectorImpl>();
}

}
