#pragma once

#include <memory>

namespace Posix {

class Fd {
public:
	static std::shared_ptr<Fd> create(int);

	virtual int get() const = 0;
	virtual size_t write(void const*, size_t) const = 0;
	virtual size_t read(void *, size_t) const = 0;

	virtual ~Fd() = default;
};

}
