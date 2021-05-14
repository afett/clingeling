#include "utest/macros.h"

#include "io/buffer.h"
#include "io/stream-buffer.h"

namespace unittests {
namespace io_stream_buffer {

UTEST_CASE(get_test)
{
	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};

	auto data = std::string_view("13:Hello, world!,");
	buf.reserve(data.size());
	data.copy(static_cast<char *>(buf.wstart()), data.size());
	buf.fill(data.size());

	auto remaining = stream.size();
	UTEST_ASSERT_EQUAL(stream.size(), remaining);
	UTEST_ASSERT_EQUAL(int('1'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('3'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int(':'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('H'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('e'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('l'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('l'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('o'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int(','), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int(' '), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('w'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('o'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('r'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('l'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('d'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int('!'), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(int(','), stream.get());
	UTEST_ASSERT_EQUAL(stream.size(), --remaining);
	UTEST_ASSERT_EQUAL(size_t(0), remaining);
	UTEST_ASSERT_EQUAL(IO::StreamBuffer::End, stream.get());
	UTEST_ASSERT_EQUAL(size_t(0), remaining);
}

UTEST_CASE(peek_test)
{
	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};

	auto data = std::string_view("13:Hello, world!,");
	buf.reserve(data.size());
	data.copy(static_cast<char *>(buf.wstart()), data.size());
	buf.fill(data.size());
	UTEST_ASSERT_EQUAL(data.size(), stream.size());

	UTEST_ASSERT_EQUAL(int('1'), stream.peek(0));
	UTEST_ASSERT_EQUAL(int('3'), stream.peek(1));
	UTEST_ASSERT_EQUAL(int(':'), stream.peek(2));
	UTEST_ASSERT_EQUAL(int('H'), stream.peek(3));
	UTEST_ASSERT_EQUAL(int('e'), stream.peek(4));
	UTEST_ASSERT_EQUAL(int('l'), stream.peek(5));
	UTEST_ASSERT_EQUAL(int('l'), stream.peek(6));
	UTEST_ASSERT_EQUAL(int('o'), stream.peek(7));
	UTEST_ASSERT_EQUAL(int(','), stream.peek(8));
	UTEST_ASSERT_EQUAL(int(' '), stream.peek(9));
	UTEST_ASSERT_EQUAL(int('w'), stream.peek(10));
	UTEST_ASSERT_EQUAL(int('o'), stream.peek(11));
	UTEST_ASSERT_EQUAL(int('r'), stream.peek(12));
	UTEST_ASSERT_EQUAL(int('l'), stream.peek(13));
	UTEST_ASSERT_EQUAL(int('d'), stream.peek(14));
	UTEST_ASSERT_EQUAL(int('!'), stream.peek(15));
	UTEST_ASSERT_EQUAL(int(','), stream.peek(16));
	UTEST_ASSERT_EQUAL(IO::StreamBuffer::End, stream.peek(17));
	UTEST_ASSERT_EQUAL(IO::StreamBuffer::End, stream.peek(18));
	UTEST_ASSERT_EQUAL(data.size(), stream.size());
}

}}
