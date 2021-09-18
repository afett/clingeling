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
	virtual std::shared_ptr<Socket> make_socket(std::tuple<int, int, int> const&, Fd::Options const&) const = 0;
	virtual std::shared_ptr<Socket> make_socket(Params const&) const = 0;
	virtual std::shared_ptr<StreamSocket> make_stream_socket(Params const&) const = 0;
	virtual ~SocketFactory() = default;
};

}
