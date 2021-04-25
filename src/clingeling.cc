#include "epoll/ctrl.h"
#include "epoll/event.h"
#include "posix/socket.h"
#include "posix/inet-address.h"
#include "posix/socket-address.h"
#include "io/buffer.h"
#include "posix/fd.h"
#include "posix/pipe-factory.h"
#include "fmt.h"
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
			true, true})}
	{ }

	State get_state() const
	{
		return state_;
	}

	void bind(Posix::SocketAddress const& addr) const override
	{
		socket_->bind(addr);
	}

	std::shared_ptr<Posix::Fd> get_fd() const override
	{
		return socket_->get_fd();
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

class StreamBuffer {
public:
	static constexpr int End = -1;

	StreamBuffer(IO::ReadBuffer & buf)
	:
		buf_(buf)
	{ }

	IO::ReadBuffer & buffer()
	{
		return buf_;
	}

	int get()
	{
		if (buf_.empty()) {
			return End;
		}

		auto res = *static_cast<char*>(buf_.rstart());
		buf_.drain(sizeof(char));
		return res;
	}

	std::string get_str(size_t len)
	{
		auto res = std::string{static_cast<char*>(buf_.rstart()), len};
		buf_.drain(len);
		return res;
	}

	int peek(size_t offs)
	{
		if (buf_.rsize() < offs) {
			return End;
		}

		return *static_cast<char*>(buf_.rstart()) + offs;
	}

private:
	IO::ReadBuffer & buf_;
};

class NetstringReader {
public:
	explicit NetstringReader(StreamBuffer & buf)
	:
		buf_(buf)
	{ }

	bool parse(std::string & str)
	{
		switch (state_) {
		case State::length:
			if (parse_length()) {
				state_ = State::string;
			}
			break;
		case State::string:
			if (parse_string()) {
				state_ = State::delim;
			}
			break;
		case State::delim:
			if (parse_delim()) {
				str = buf_.get_str(len_);
				len_ = 0;
				state_ = State::length;
				return true;
			}
			break;
		}

		return false;
	}

private:
	enum class State {
		length,
		string,
		delim,
	};

	bool parse_length()
	{
		auto c = buf_.get();
		switch (c) {
		case StreamBuffer::End:
			break;
		case '0' ... '9':
			len_ = len_ * 10 + c - '0';
			break;
		case ':':
			return true;
		default:
			throw std::runtime_error(Fmt::format("unexpected character '%s' while parsing length", char(c)));
		}
		return false;
	}

	bool parse_string() const
	{
		return buf_.buffer().rsize() >= len_ + 1;
	}

	bool parse_delim()
	{
		auto c = buf_.peek(len_ + 1);
		switch (c) {
		case ',':
			// TODO
			return true;
		case StreamBuffer::End:
			throw std::runtime_error("unexpected buffer end");
		default:
			throw std::runtime_error(Fmt::format("unexpected character '%s' while parsing delimiter", char(c)));
		}
		return false;
	}

	StreamBuffer & buf_;
	State state_ = State::length;
	size_t len_ = 0;
};

int clingeling(int, char *[])
{
	auto socket_factory = Posix::SocketFactory::create();
	auto socket = std::make_shared<StreamSocket>(*socket_factory);

	socket->connect(Posix::SocketAddress{Posix::Inet::Address{"127.0.0.1"}, 4444});

	std::cerr << socket->get_state() << "\n";

	EPoll::Event ev{EPoll::Event::Type::In};
	switch (socket->get_state()) {
	case StreamSocket::State::in_progress:
		ev = EPoll::Event::Type::Out;
		// fallthrough
	case StreamSocket::State::connected:
		break;
	case StreamSocket::State::error:
		throw std::runtime_error("Socket in state Error");
	case StreamSocket::State::init:
		throw std::runtime_error("Socket in state Init");
	}

	auto buf = IO::Buffer{4096};
	auto stream = StreamBuffer{buf};
	auto netstring = NetstringReader{stream};

	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	poller->add(socket->get_fd(), ev, [&netstring, &socket, &buf, &poller](auto ev) {
		if (!ev) {
			return;
		}

		if (socket->get_state() == StreamSocket::State::in_progress) {
			socket->connect_continue();
			poller->mod(socket->get_fd(), EPoll::Event::Type::In);
			return;
		}

		if (ev & EPoll::Event::Type::Out) {
			throw std::runtime_error("POLLOUT");
		}
		if (ev & EPoll::Event::Type::RdHup) {
			throw std::runtime_error("RDHUP");
		}
		if (ev & EPoll::Event::Type::Pri) {
			throw std::runtime_error("PRI");
		}
		if (ev & EPoll::Event::Type::Err) {
			throw std::runtime_error("ERR");
		}
		if (ev & EPoll::Event::Type::Hup) {
			throw std::runtime_error("HUP");
		}
		if (ev != EPoll::Event::Type::In) {
			throw std::runtime_error("bad epoll event");
		}

		auto res = socket->get_fd()->read(buf.wstart(), buf.wsize());
		if (res == 0) {
			// FIXME
			throw std::runtime_error("Connection closed");
		}

		auto json = std::string{};
		if (netstring.parse(json)) {
			std::cout << json << "\n";
		}

		poller->mod(socket->get_fd(), buf.full() ? EPoll::Event{} : EPoll::Event::Type::In);
	});

	auto pipe_factory = Posix::PipeFactory::create();
	auto pipe = pipe_factory->make_pipe(Posix::PipeFactory::Params{false, true});

	auto run{true};
	poller->add(std::get<0>(pipe), EPoll::Event::Type::In, [&run, &pipe](auto ev) {
		if (ev != EPoll::Event::Type::In) {
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
