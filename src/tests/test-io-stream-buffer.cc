#include <cppunit/extensions/HelperMacros.h>
#include "io/buffer.h"
#include "io/stream-buffer.h"

namespace unittests {
namespace io_stream_buffer {

class test : public CppUnit::TestCase {
public:
	test();
	void setUp();
	void tearDown();

private:
	void get_test();
	void peek_test();

	CPPUNIT_TEST_SUITE(test);
	CPPUNIT_TEST(get_test);
	CPPUNIT_TEST(peek_test);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(test);

test::test()
{ }

void test::setUp()
{ }

void test::tearDown()
{ }

void test::get_test()
{
	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};

	auto data = std::string_view("13:Hello, world!,");
	buf.reserve(data.size());
	data.copy(static_cast<char *>(buf.wstart()), data.size());
	buf.fill(data.size());

	auto remaining = stream.size();
	CPPUNIT_ASSERT_EQUAL(stream.size(), remaining);
	CPPUNIT_ASSERT_EQUAL(int('1'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('3'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int(':'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('H'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('e'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('l'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('l'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('o'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int(','), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int(' '), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('w'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('o'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('r'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('l'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('d'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int('!'), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(int(','), stream.get());
	CPPUNIT_ASSERT_EQUAL(stream.size(), --remaining);
	CPPUNIT_ASSERT_EQUAL(size_t(0), remaining);
	CPPUNIT_ASSERT_EQUAL(IO::StreamBuffer::End, stream.get());
	CPPUNIT_ASSERT_EQUAL(size_t(0), remaining);
}

void test::peek_test()
{
	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};

	auto data = std::string_view("13:Hello, world!,");
	buf.reserve(data.size());
	data.copy(static_cast<char *>(buf.wstart()), data.size());
	buf.fill(data.size());
	CPPUNIT_ASSERT_EQUAL(data.size(), stream.size());

	CPPUNIT_ASSERT_EQUAL(int('1'), stream.peek(0));
	CPPUNIT_ASSERT_EQUAL(int('3'), stream.peek(1));
	CPPUNIT_ASSERT_EQUAL(int(':'), stream.peek(2));
	CPPUNIT_ASSERT_EQUAL(int('H'), stream.peek(3));
	CPPUNIT_ASSERT_EQUAL(int('e'), stream.peek(4));
	CPPUNIT_ASSERT_EQUAL(int('l'), stream.peek(5));
	CPPUNIT_ASSERT_EQUAL(int('l'), stream.peek(6));
	CPPUNIT_ASSERT_EQUAL(int('o'), stream.peek(7));
	CPPUNIT_ASSERT_EQUAL(int(','), stream.peek(8));
	CPPUNIT_ASSERT_EQUAL(int(' '), stream.peek(9));
	CPPUNIT_ASSERT_EQUAL(int('w'), stream.peek(10));
	CPPUNIT_ASSERT_EQUAL(int('o'), stream.peek(11));
	CPPUNIT_ASSERT_EQUAL(int('r'), stream.peek(12));
	CPPUNIT_ASSERT_EQUAL(int('l'), stream.peek(13));
	CPPUNIT_ASSERT_EQUAL(int('d'), stream.peek(14));
	CPPUNIT_ASSERT_EQUAL(int('!'), stream.peek(15));
	CPPUNIT_ASSERT_EQUAL(int(','), stream.peek(16));
	CPPUNIT_ASSERT_EQUAL(IO::StreamBuffer::End, stream.peek(17));
	CPPUNIT_ASSERT_EQUAL(IO::StreamBuffer::End, stream.peek(18));
	CPPUNIT_ASSERT_EQUAL(data.size(), stream.size());
}

}}
