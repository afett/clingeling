#pragma once

#include <cstddef>
#include <memory>

namespace Posix {

class Fd;

class Writer {
public:
	virtual size_t write(void const*, size_t) const = 0;
	virtual Fd getFd() const = 0;
	virtual ~Writer() = default;
};

class WriterFactory {
public:
	static std::unique_ptr<WriterFactory> create();
	virtual std::unique_ptr<Writer> make_writer(Fd const&) const = 0;
	virtual ~WriterFactory() = default;
};

}
