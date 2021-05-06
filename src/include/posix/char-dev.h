#pragma once

#include <posix/fd.h>

namespace Posix {

class CharDev : public Fd {
public:
	virtual void ioctl(unsigned long, void*) const = 0;
};

class CharDevFactory {
public:
	static std::unique_ptr<CharDevFactory> create();
	virtual std::shared_ptr<CharDev> make_chardev(std::string const&) const = 0;
	virtual ~CharDevFactory() = default;
};

}
