#pragma once

#include <string>

namespace IO {

class ReadBuffer;

class StreamBuffer {
public:
	static constexpr int End = -1;

	StreamBuffer(IO::ReadBuffer &);
	IO::ReadBuffer & buffer();
	int get();
	std::string get_str(size_t);
	int peek(size_t);

private:
	IO::ReadBuffer & buf_;
};

}
