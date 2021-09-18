/*
   Copyright (c) 2021 Andreas Fett
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include <memory>

struct sockaddr;
struct sockaddr_in;
struct sockaddr_in6;
struct sockaddr_storage;

namespace Posix {

namespace Inet {
class Address;
}

class SocketAddress {
public:
	SocketAddress();

	SocketAddress(std::unique_ptr<sockaddr_storage>, size_t);
	SocketAddress(Inet::Address const&, uint16_t);
	SocketAddress(SocketAddress const&);
	SocketAddress(SocketAddress &&);
	SocketAddress & operator=(SocketAddress const&);
	SocketAddress & operator=(SocketAddress &&);
	~SocketAddress();

	sockaddr const* getSockaddr() const
	{
		return reinterpret_cast<sockaddr const*>(data_.get());
	}

	size_t size() const
	{
		return size_;
	}

	explicit operator bool() const
	{
		return size_ != 0;
	}

	friend bool operator<(SocketAddress const&, SocketAddress const&);
	friend std::string to_string(SocketAddress const&);

private:
	sockaddr_in * getSockaddrIn()
	{
		return reinterpret_cast<sockaddr_in *>(data_.get());
	}

	sockaddr_in6 * getSockaddrIn6()
	{
		return reinterpret_cast<sockaddr_in6 *>(data_.get());
	}

	sockaddr_in * getSockaddrIn() const
	{
		return reinterpret_cast<sockaddr_in *>(data_.get());
	}

	sockaddr_in6 * getSockaddrIn6() const
	{
		return reinterpret_cast<sockaddr_in6 *>(data_.get());
	}

	size_t size_ = 0;
	std::unique_ptr<sockaddr_storage> data_;
};

inline bool operator>(SocketAddress const& l, SocketAddress const& r)
{
	return r < l;
}

inline bool operator==(SocketAddress const& l, SocketAddress const& r)
{
	return !(l < r) && !(l > r);
}

inline bool operator!=(SocketAddress const& l, SocketAddress const& r)
{
	return !(l == r);
}

inline bool operator<=(SocketAddress const& l, SocketAddress const& r)
{
	return (l < r) || (l == r);
}

inline bool operator>=(SocketAddress const& l, SocketAddress const& r)
{
	return (l > r) || (l == r);
}

std::string to_string(SocketAddress const&);

}
