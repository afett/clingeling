
#include "epoll/ctrl.h"
#include "epoll/event.h"
#include "posix/socket-factory.h"
#include "posix/connector.h"
#include "posix/inet-address.h"
#include "posix/socket-address.h"
#include "io/buffer.h"
#include "posix/reader.h"
#include "posix/fd.h"
#include "posix/pipe-factory.h"

int clingeling(int, char *[])
{

	auto socket_factory = Posix::SocketFactory::create();
	auto fd = socket_factory->make_socket({
			Posix::SocketFactory::Params::Domain::Inet,
			Posix::SocketFactory::Params::Type::Stream,
			true, true});

	auto connector_factory = Posix::ConnectorFactory::create();
	auto connector = connector_factory->make_connector();
	connector->connect(fd, Posix::SocketAddress{Posix::Inet::Address{"127.0.0.1"}, 4444});

	auto reader_factory = Posix::ReaderFactory::create();
	auto reader = reader_factory->make_reader(fd);

	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	auto buf = IO::Buffer{4096};
	poller->add(fd, EPoll::Event::Type::In, [&buf, &reader, &poller](auto ev) {
		if (ev != EPoll::Event::Type::In) {
			throw std::runtime_error("bad epoll event");
		}

		auto res = reader->read(buf.wstart(), buf.wsize());
		if (res == 0) {
			// FIXME
			throw std::runtime_error("Connection closed");
		}

		poller->mod(reader->getFd(), buf.full() ? EPoll::Event{} : EPoll::Event::Type::In);
	});

	auto pipe_factory = Posix::PipeFactory::create();
	auto pipe = pipe_factory->make_pipe(Posix::PipeFactory::Params{false, true});
	auto pipe_reader = reader_factory->make_reader(pipe.first);

	auto run{true};
	poller->add(pipe.first, EPoll::Event::Type::In, [&run, &pipe_reader](auto ev) {
		if (ev != EPoll::Event::Type::In) {
			throw std::runtime_error("bad epoll event");
		}

		int wakeup{0};
		auto res = pipe_reader->read(&wakeup, sizeof(wakeup));
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
