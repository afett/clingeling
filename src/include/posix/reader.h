#pragma once

#include <cstddef>
#include <memory>

namespace Posix {

class Fd;

class Reader {
public:
	virtual size_t read(void *, size_t) const = 0;
	virtual Fd getFd() const = 0;
	virtual ~Reader() = default;
};

class ReaderFactory {
public:
	static std::unique_ptr<ReaderFactory> create();
	virtual std::unique_ptr<Reader> make_reader(Fd const&) const = 0;
	virtual ~ReaderFactory() = default;
};

}
