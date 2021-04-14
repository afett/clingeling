#include "log.h"
#include "epoll/ctrl.h"
#include "epoll/event.h"
#include "posix/socket-factory.h"
#include "posix/socket-address.h"
#include "posix/reader.h"
#include "posix/fd.h"
#include "posix/connector.h"
#include "posix/inet-address.h"

#include <iostream>

int main()
{
	Log::Logger logger;
	logger.on_message([](Log::Message const& msg) {
		std::cerr << msg.get_msg() << "\n";
		});

	logger.post(Log::Message("Hello"));

	auto socket_factory = Posix::SocketFactory::create();

	auto fd = socket_factory->make_socket({
		Posix::SocketFactory::Params::Domain::Inet,
		Posix::SocketFactory::Params::Type::Stream,
		true,
		true});

	auto connector_factory = Posix::ConnectorFactory::create();
	auto connector = connector_factory->make_connector();
	connector->connect(fd, Posix::SocketAddress{Posix::Inet::Address{"127.0.0.1"}, 4444});

	auto reader_factory = Posix::ReaderFactory::create();
	auto reader = reader_factory->make_reader(fd);

	auto ctrl_factory = EPoll::CtrlFactory::create();
	auto ctrl = ctrl_factory->make_ctrl();

	ctrl->add(reader->getFd(), EPoll::Event::Type::In, [&logger, &reader](EPoll::Event const&) {
				auto buf = std::array<char, 4096>{};
				auto read = reader->read(buf.data(), buf.size());
				logger.post(Log::make_message("Read: %s", read));
			});

	ctrl->wait();
	return 0;
}
