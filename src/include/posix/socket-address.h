#pragma once

#include <memory>

struct sockaddr;
struct sockaddr_storage;

namespace Posix {

namespace Inet {
class Address;
}

class SocketAddress {
public:
	SocketAddress(Inet::Address const&, uint16_t);

	sockaddr const* getSockaddr() const
	{
		return reinterpret_cast<sockaddr const*>(data_.get());
	}

	size_t size() const
	{
		return size_;
	}

	~SocketAddress();

private:
	size_t size_ = 0;
	std::unique_ptr<sockaddr_storage> data_;
};

}
