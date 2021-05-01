#include <cppunit/extensions/HelperMacros.h>
#include "netstring/reader.h"
#include "io/buffer.h"
#include "io/stream-buffer.h"

namespace unittests {
namespace netstring_reader {

class test : public CppUnit::TestCase {
public:
	test();
	void setUp();
	void tearDown();

private:
	void simple_test();

	CPPUNIT_TEST_SUITE(test);
	CPPUNIT_TEST(simple_test);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(test);

test::test()
{ }

void test::setUp()
{ }

void test::tearDown()
{ }

void test::simple_test()
{
	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};
	auto ns_reader = Netstring::Reader{stream};
	auto netstr = std::string_view("13:Hello, world!,");
	buf.reserve(netstr.size());
	netstr.copy(static_cast<char *>(buf.wstart()), netstr.size());
	buf.fill(netstr.size());

	std::string res;
	CPPUNIT_ASSERT(ns_reader.parse(res));
	CPPUNIT_ASSERT_EQUAL(std::string("Hello, world!"), res);
	CPPUNIT_ASSERT_EQUAL(IO::StreamBuffer::End, stream.get());
}

}}
