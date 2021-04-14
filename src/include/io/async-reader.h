#pragma once

#include <memory>

namespace Posix {
class Reader;
}

namespace IO {

class AsyncReader {
public:
	virtual void read(size_t, std::function<void(size_t, void*)>) = 0;
	virtual ~AsyncReader() = default;
};

class AsyncReaderFactory {
public:
	static std::unique_ptr<AsyncReaderFactory> create();
	virtual std::unique_ptr<AsyncReader> make_reader(std::unique_ptr<Posix::Reader>) = 0;
};

}
