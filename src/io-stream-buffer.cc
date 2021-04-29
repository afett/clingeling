#include "io/buffer.h"
#include "io/stream-buffer.h"

namespace IO {

StreamBuffer::StreamBuffer(IO::ReadBuffer & buf)
:
	buf_(buf)
{ }

IO::ReadBuffer & StreamBuffer::buffer()
{
	return buf_;
}

int StreamBuffer::get()
{
	if (buf_.empty()) {
		return End;
	}

	auto res = *static_cast<char*>(buf_.rstart());
	buf_.drain(sizeof(char));
	return res;
}

std::string StreamBuffer::get_str(size_t len)
{
	auto res = std::string{static_cast<char*>(buf_.rstart()), len};
	buf_.drain(len);
	return res;
}

int StreamBuffer::peek(size_t offs) const
{
	if (buf_.rsize() < offs) {
		return End;
	}

	return *static_cast<char*>(buf_.rstart()) + offs;
}

size_t StreamBuffer::size() const
{
	return buf_.rsize();
}

}
