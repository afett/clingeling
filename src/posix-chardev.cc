/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include "posix/char-dev.h"
#include "posix/system-error.h"

#include <sys/ioctl.h>
#include <sys/fcntl.h>

namespace Posix {

class CharDevImpl : public CharDev {
public:
	explicit CharDevImpl(std::shared_ptr<Fd> const&);

	void ioctl(unsigned long, void*) const override;

	int get() const override
	{
		return fd_->get();
	}

	size_t write(void const* buf, size_t count) const override
	{
		return fd_->write(buf, count);
	}

	size_t read(void * buf, size_t count) const override
	{
		return fd_->read(buf, count);
	}

private:
	std::shared_ptr<Fd> fd_;
};

CharDevImpl::CharDevImpl(std::shared_ptr<Fd> const& fd)
:
	fd_(fd)
{ }

void CharDevImpl::ioctl(unsigned long request, void *argp) const
{
	int res{-1};
	do {
		res = ::ioctl(fd_->get(), request, argp);
	} while (res == -1 && errno == EINTR);

	if (res == -1) {
		throw POSIX_SYSTEM_ERROR("::ioctl(%s, %s, %x)", fd_.get(), request, argp);
	}
}

class CharDevFactoryImpl : public CharDevFactory {
public:
	static std::unique_ptr<CharDevFactory> create();
	std::shared_ptr<CharDev> make_chardev(std::string const&) const override;
};

std::unique_ptr<CharDevFactory> CharDevFactory::create()
{
	return std::make_unique<CharDevFactoryImpl>();
}

std::shared_ptr<CharDev> CharDevFactoryImpl::make_chardev(std::string const& path) const
{
	auto fd = ::open(path.c_str(), O_CLOEXEC|O_NONBLOCK|O_RDWR);
	if (fd == -1) {
		throw make_system_error(errno, Fmt::format("::open(%s, O_CLOEXEC|O_NONBLOCK|O_RDWR);", path.c_str()));
	}
	return std::make_shared<CharDevImpl>(Fd::create(fd));
}

}
