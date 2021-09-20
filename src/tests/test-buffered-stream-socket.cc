#include "utest/macros.h"

#include "buffered-stream-socket.h"
#include "posix/socket.h"
#include "posix/inet-address.h"
#include "posix/socket-address.h"

namespace EPoll {

class CtrlMock : public Ctrl {
public:
	void add(std::shared_ptr<Posix::Fd> const& fd, Events const& ev, std::function<void(Events const&)> const& cb) override
	{
		add_.emplace_back(fd, ev, cb);
	}

	void del(std::shared_ptr<Posix::Fd> const&) override
	{
	}

	void mod(std::shared_ptr<Posix::Fd> const& fd, Events const& ev) const override
	{
		mod_.emplace_back(fd, ev);
	}

	bool wait(std::chrono::milliseconds const& = Infinity()) const override
	{
		return true;
	}

	std::vector<std::tuple<std::shared_ptr<Posix::Fd>, Events, std::function<void(Events const&)>>> add_;
	mutable std::vector<std::tuple<std::shared_ptr<Posix::Fd>, Events>> mod_;
};

}

namespace Posix {

class StreamSocketMock : public StreamSocket {
public:
	void bind(SocketAddress const&) const override
	{
	}

	void connect(SocketAddress const& addr) const override
	{
		connect_.push_back(addr);
	}

	std::error_code get_socket_error() const override
	{
		return std::error_code{};
	}

	void set_recvbuf(size_t) const override
	{
	}

	State state() const override
	{
		return State::in_progress;
	}

	void connect_continue() override
	{
	}

	int get() const override
	{
		return 0;
	}

	size_t write(void const*, size_t) const override
	{
		return 0;
	}

	size_t read(void *, size_t) const override
	{
		return 0;
	}

	mutable std::vector<SocketAddress> connect_;
};

class SocketFactoryMock : public SocketFactory {
public:
	std::shared_ptr<Socket> make_socket(std::tuple<int, int, int> const&, Fd::Options const&) const override
	{
		return {};
	}

	std::shared_ptr<Socket> make_socket(Params const&) const override
	{
		return {};
	}

	std::shared_ptr<StreamSocket> make_stream_socket(Params const& params) const override
	{
		auto res{std::make_shared<StreamSocketMock>()};
		make_stream_socket_.emplace_back(params ,res);
		return res;
	}

	mutable std::vector<std::tuple<Params, std::weak_ptr<StreamSocketMock>>> make_stream_socket_;
};

}

namespace unittests {
namespace buffered_stream_socket {

class Fixture {
public:
	Fixture()
	:
		epoll(),
		socket_factory(),
		sock(epoll, socket_factory, Posix::SocketAddress{Posix::Inet::Address{"127.0.0.1"}, 4444})
	{ }

	EPoll::CtrlMock epoll;
	Posix::SocketFactoryMock socket_factory;
	BufferedStreamSocket sock;
};

UTEST_CASE_WITH_FIXTURE(simple_test, Fixture)
{

}

}}
