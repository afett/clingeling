#include "epoll/ctrl.h"
#include "posix/socket.h"
#include "posix/inet-address.h"
#include "posix/socket-address.h"
#include "io/buffer.h"
#include "io/stream-buffer.h"
#include "posix/fd.h"
#include "posix/pipe-factory.h"
#include "fmt.h"
#include "netstring/reader.h"
#include <system_error>
#include <iostream>

class StreamSocket : public Posix::Socket {
public:
	enum class State {
		init,
		in_progress,
		connected,
		error,
	};

	explicit StreamSocket(Posix::SocketFactory & socket_factory)
	:
		socket_{socket_factory.make_socket({
			Posix::SocketFactory::Params::Domain::Inet,
			Posix::SocketFactory::Params::Type::Stream,
			Posix::Fd::Option::nonblock|Posix::Fd::Option::cloexec})}
	{ }

	State get_state() const
	{
		return state_;
	}

	void bind(Posix::SocketAddress const& addr) const override
	{
		socket_->bind(addr);
	}

	int get() const override
	{
		return socket_->get();
	}

	size_t write(void const* buf, size_t count) const override
	{
		return socket_->write(buf, count);
	}

	size_t read(void *buf, size_t count) const override
	{
		return socket_->read(buf, count);
	}

	std::error_code get_socket_error() const override
	{
		return socket_->get_socket_error();
	}

	void connect(Posix::SocketAddress const& addr) const override
	{
		try {
			socket_->connect(addr);
		} catch (std::system_error const& e) {
			if (e.code() == std::errc::operation_in_progress) {
				state_ = State::in_progress;
				return;
			} else {
				state_ = State::error;
				throw;
			}
		}
		state_ = State::connected;
	}

	void connect_continue()
	{
		if (state_ != State::in_progress) {
			throw std::runtime_error("Socket connect not in progress");
		}

		auto ec{socket_->get_socket_error()};
		if (ec) {
			throw std::system_error(ec);
		}

		state_ = State::connected;
	}

private:
	mutable State state_ = State::init;
	std::shared_ptr<Posix::Socket> socket_;
};

std::ostream & operator<<(std::ostream & os, StreamSocket::State state)
{
	switch (state) {
	case StreamSocket::State::init: return os << "init";
	case StreamSocket::State::in_progress: return os << "in_progress";
	case StreamSocket::State::connected: return os << "connected";
	case StreamSocket::State::error: return os << "error";
	}
	return os;
}

int clingeling(int, char *[])
{
	auto socket_factory = Posix::SocketFactory::create();
	auto socket = std::make_shared<StreamSocket>(*socket_factory);

	socket->connect(Posix::SocketAddress{Posix::Inet::Address{"127.0.0.1"}, 4444});

	std::cerr << socket->get_state() << "\n";

	EPoll::Events ev{EPoll::Event::In};
	switch (socket->get_state()) {
	case StreamSocket::State::in_progress:
		ev = EPoll::Event::Out;
		// fallthrough
	case StreamSocket::State::connected:
		break;
	case StreamSocket::State::error:
		throw std::runtime_error("Socket in state Error");
	case StreamSocket::State::init:
		throw std::runtime_error("Socket in state Init");
	}

	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};
	auto netstring = Netstring::Reader{stream};

	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	poller->add(socket, ev, [&netstring, &socket, &buf, &poller](auto ev) {
		if (!ev) {
			return;
		}

		if (socket->get_state() == StreamSocket::State::in_progress) {
			socket->connect_continue();
			poller->mod(socket, EPoll::Events{EPoll::Event::In});
			return;
		}

		if (ev & EPoll::Event::Out) {
			throw std::runtime_error("POLLOUT");
		}
		if (ev & EPoll::Event::RdHup) {
			throw std::runtime_error("RDHUP");
		}
		if (ev & EPoll::Event::Pri) {
			throw std::runtime_error("PRI");
		}
		if (ev & EPoll::Event::Err) {
			throw std::runtime_error("ERR");
		}
		if (ev & EPoll::Event::Hup) {
			throw std::runtime_error("HUP");
		}
		if (ev != EPoll::Event::In) {
			throw std::runtime_error("bad epoll event");
		}

		auto res = socket->read(buf.wstart(), buf.wsize());
		if (res == 0) {
			// FIXME
			throw std::runtime_error("Connection closed");
		}

		auto json = std::string{};
		if (netstring.parse(json)) {
			std::cout << json << "\n";
		}

		poller->mod(socket, buf.full() ? EPoll::Events{} : EPoll::Events{EPoll::Event::In});
	});

	auto pipe_factory = Posix::PipeFactory::create();
	auto pipe = pipe_factory->make_pipe(Posix::PipeFactory::Params{false, true});

	auto run{true};
	poller->add(std::get<0>(pipe), EPoll::Events{EPoll::Event::In}, [&run, &pipe](auto ev) {
		if (ev != EPoll::Event::In) {
			throw std::runtime_error("bad epoll event");
		}

		int wakeup{0};
		auto res = std::get<0>(pipe)->read(&wakeup, sizeof(wakeup));
		if (res == sizeof(wakeup)) {
			run = false;
			return;
		}
		throw std::runtime_error("bad read from pipe");
	});

	do {
		poller->wait();
	} while (run);

	return 0;
}
