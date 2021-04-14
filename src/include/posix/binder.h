#pragma once

#include <memory>

namespace Posix {

class Fd;
class SocketAddress;

class Binder {
public:
	virtual void bind(Fd const&, SocketAddress const&) const = 0;
	virtual ~Binder() = default;
};

class BinderFactory {
public:
	static std::unique_ptr<BinderFactory> create();
	virtual std::unique_ptr<Binder> make_binder() const = 0;
	virtual ~BinderFactory() = default;
};

}
