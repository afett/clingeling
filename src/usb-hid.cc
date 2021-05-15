/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include "usb/hid.h"

#include "posix/char-dev.h"

#include <linux/hiddev.h>

namespace USB {
namespace Hid {

class DeviceImpl : public Device {
public:
	explicit DeviceImpl(std::shared_ptr<Posix::CharDev> const& dev)
	:
		dev_(dev)
	{ }

	Event read_event() override;
	void write(Usage::Report) override;
	int32_t read(Usage::Report) override;

private:
	std::shared_ptr<Posix::CharDev> dev_;
};

std::shared_ptr<Device> Device::create(std::shared_ptr<Posix::CharDev> const& dev)
{
	return std::make_shared<DeviceImpl>(dev);
}

Event DeviceImpl::read_event()
{
	hiddev_event ev;
	auto res = dev_->read(&ev, sizeof(ev));
	if (res != sizeof(ev)) {
		throw std::runtime_error("DeviceImpl::read_event(): short read");
	}
	// static_assert(sizeof(ev.hid) == sizeof(Event::hid_));
	return Event{ev.hid, ev.value};
}

void DeviceImpl::write(Usage::Report)
{
}

int32_t DeviceImpl::read(Usage::Report)
{
	return 0;
}

}}
