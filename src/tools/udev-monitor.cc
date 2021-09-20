#include "epoll/ctrl.h"
#include "source-location.h"

#include "netlink/socket.h"
#include "posix/system-error.h"

#include <iostream>
#include <cctype>

namespace {

void on_netlink_event(EPoll::Events const& ev, std::shared_ptr<Netlink::Socket> const& socket)
{
	if (ev != EPoll::Event::In) {
		auto ec = socket->get_socket_error();
		throw Posix::make_system_error(ec.value(), "bad epoll event");
	}

	auto buf = std::array<char, 8192>{};
	auto size = socket->read(buf.data(), buf.size());

	if (size == buf.size() || size < 5) {
		return;
	}

	auto end = std::begin(buf) + size;
	if (*std::prev(end) != '\0') {
		return;
	}

	// bind@/devices/pci0000:00/0000:00:14.0/usb1/1-2/1-2:1.0.ACTION=bind.DEVPATH=/devices/pci0000:00/0000:00:14.0/usb1/1-2/1-2:1.0.SUBSYSTEM=usb.DEVTYPE=usb_interface.DRIVER=usbhid.PRODUCT=b0e/e44/120.TYPE=0/0/0.INTERFACE=3/0/0.MODALIAS=usb:v0B0Ep0E44d0120dc00dsc00dp00ic03isc00ip00in00.SEQNUM=4424.

	for (auto i{0U}; i < size; ++i) {
		if (buf[i] == '\0') {
			buf[i] = '\n';
		} else if (!std::isprint(buf[i])) {
			buf[i] = '.';
		}
	}
	std::cerr << "\n----- MESSAGE BEGIN -----\n";
	std::cerr << buf.data();
	std::cerr << "----- MESSAGE END -----\n";
}

void backtrace(std::exception const& e)
{
	std::cerr << e.what() << "\n";
	try {
		std::rethrow_if_nested(e);
	} catch (std::exception const& e) {
		backtrace(e);
	}
}

}

int main()
{
	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	auto netlink_socket_factory = Netlink::SocketFactory::create();
	auto netlink_socket = netlink_socket_factory->make_socket(
			Netlink::Family::KObjectUEvent, {Posix::Fd::Option::nonblock|Posix::Fd::Option::cloexec});
	netlink_socket->set_recvbuf(128*1024*1024);
	netlink_socket->bind(0x0001);

	poller->add(netlink_socket, EPoll::Events{EPoll::Event::In},
			[&netlink_socket](auto const& ev) { ::on_netlink_event(ev, netlink_socket); });

	try {
		do {
			poller->wait();
		} while (true);
	} catch (std::exception const& e) {
		::backtrace(e);
		return 1;
	}

	return 0;
}
