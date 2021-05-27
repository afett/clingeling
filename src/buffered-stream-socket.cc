#include "buffered-stream-socket.h"

#include "posix/socket.h"

BufferedStreamSocket::BufferedStreamSocket(
	EPoll::Ctrl & poller,
	Posix::SocketFactory & socket_factory,
	Posix::SocketAddress const& addr)
:
	sendbuf_{4096},
	recvbuf_{4096},
	socket_(socket_factory.make_stream_socket({
		Posix::SocketFactory::Params::Domain::Inet,
		Posix::SocketFactory::Params::Type::Stream,
		Posix::Fd::Option::nonblock|Posix::Fd::Option::cloexec})),
	ev_{poll_events()},
	poller_(poller)
{
	connect(addr);

	recvbuf_.on_drain([this] () { update_poll_events(); });

	poller.add(socket_, ev_, [this] (auto ev) {
		if (!ev) {
			return;
		}

		if (socket_->state() == Posix::StreamSocket::State::in_progress) {
			socket_->connect_continue();
			update_poll_events();
			return;
		}

		dispatch(ev, {
			{EPoll::Event::RdHup, [] () { throw std::runtime_error("RDHUP"); }},
			{EPoll::Event::Pri,   [] () { throw std::runtime_error("PRI"); }},
			{EPoll::Event::Err,   [] () { throw std::runtime_error("ERR"); }},
			{EPoll::Event::Hup,   [] () { throw std::runtime_error("HUP"); }},
			{EPoll::Event::In,    [this] () { on_readable(); }},
			{EPoll::Event::Out,   [this] () { on_writable(); }}
		});

		update_poll_events();
	});
}

void BufferedStreamSocket::on_readable()
{
	auto size = socket_->read(recvbuf_.wstart(), recvbuf_.wsize());
	if (size == 0) {
		// FIXME
		throw std::runtime_error("Connection closed");
	}
	recvbuf_.fill(size);
}

void BufferedStreamSocket::on_writable()
{
	auto size = socket_->write(sendbuf_.rstart(), sendbuf_.rsize());
	sendbuf_.drain(size);
}

void BufferedStreamSocket::connect(Posix::SocketAddress const& addr)
{
	socket_->connect(addr);

	switch (socket_->state()) {
	case Posix::StreamSocket::State::init:
		throw std::runtime_error("Socket in state Init");
	case Posix::StreamSocket::State::error:
		throw std::runtime_error("Socket in state Error");
	case Posix::StreamSocket::State::in_progress:
	case Posix::StreamSocket::State::connected:
		break;
	}
}

void BufferedStreamSocket::update_poll_events()
{
	if (auto ev = poll_events(); ev != ev_) {
		ev_ = ev;
		poller_.mod(socket_, ev_);
	}
}

EPoll::Events BufferedStreamSocket::poll_events() const
{
	if (socket_->state() == Posix::StreamSocket::State::in_progress) {
		return EPoll::Events{EPoll::Event::Out};
	}

	EPoll::Events ev;
	if (!sendbuf_.empty()) {
		ev |= EPoll::Event::Out;
	}
	if (!recvbuf_.full()) {
		ev |= EPoll::Event::In;
	}
	return ev;
}
