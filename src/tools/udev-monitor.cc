#include "epoll/ctrl.h"
#include "source-location.h"

int main()
{
	auto poller_factory = EPoll::CtrlFactory::create();
	auto poller = poller_factory->make_ctrl();

	try {
		do {
			poller->wait();
		} while (true);
	} catch (...) {
		throw_backtrace();
	}

	return 0;
}
