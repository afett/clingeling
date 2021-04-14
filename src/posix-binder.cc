#include <sys/socket.h>

#include "posix/binder.h"
#include "posix/fd.h"
#include "posix/socket-address.h"
#include "posix/system-error.h"

namespace Posix {

class BinderImpl : public Binder {
public:
	void bind(Fd const&, SocketAddress const&) const override;
};

void BinderImpl::bind(Fd const& fd, SocketAddress const& addr) const
{
	auto res = ::bind(fd.get(), addr.getSockaddr(), addr.size());
	if (res == -1) {
		throw POSIX_SYSTEM_ERROR("::bind(fd.get(), addr.data(), addr.size());", fd.get(), addr.getSockaddr(), addr.size());
	}
}

class BinderFactoryImpl : public BinderFactory {
public:
	std::unique_ptr<Binder> make_binder() const override;
};

std::unique_ptr<BinderFactory> BinderFactory::create()
{
	return std::make_unique<BinderFactoryImpl>();
}

std::unique_ptr<Binder> BinderFactoryImpl::make_binder() const
{
	return std::make_unique<BinderImpl>();
}

}
