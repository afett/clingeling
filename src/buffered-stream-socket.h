#pragma once

#include <memory>

#include "io/event-buffer.h"
#include "epoll/ctrl.h"

namespace Posix {
class SocketFactory;
class SocketAddress;
class StreamSocket;
}

class BufferedStreamSocket {
public:
	BufferedStreamSocket(
		EPoll::Ctrl &,
		Posix::SocketFactory &,
		Posix::SocketAddress const&);

	IO::ReadEventBuffer & recvbuf()
	{
		return recvbuf_;
	}

	IO::WriteBuffer & sendbuf()
	{
		return sendbuf_;
	}

private:
	void on_readable();
	void on_writable();
	void connect(Posix::SocketAddress const&);
	void update_poll_events();
	EPoll::Events poll_events() const;

	IO::EventBuffer sendbuf_;
	IO::EventBuffer recvbuf_;
	EPoll::Ctrl & poller_;
	std::shared_ptr<Posix::StreamSocket> socket_;
};
