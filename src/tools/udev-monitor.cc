#include "epoll/ctrl.h"
#include "source-location.h"

#include "netlink/socket.h"

namespace {

void on_netlink_event(EPoll::Events const& ev)
{
	if (ev != EPoll::Event::In) {
		throw std::runtime_error("bad epoll event");
	}
}

}

int main()
{
	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	auto netlink_socket_factory = Netlink::SocketFactory::create();
	auto netlink_socket = netlink_socket_factory->make_socket(
			Netlink::Family::Generic, {});

	poller->add(netlink_socket, EPoll::Events{EPoll::Event::In},
			[](auto const& ev) { ::on_netlink_event(ev); });

	try {
		do {
			poller->wait();
		} while (true);
	} catch (...) {
		throw_backtrace();
	}

	return 0;
}
