#include "utest/macros.h"
#include "netstring/reader.h"
#include "io/buffer.h"
#include "io/stream-buffer.h"

namespace unittests {
namespace netstring_reader {

UTEST_CASE(simple_test)
{
	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};
	auto ns_reader = Netstring::Reader{stream};
	auto netstr = std::string_view("13:Hello, world!,");
	buf.reserve(netstr.size());
	netstr.copy(static_cast<char *>(buf.wstart()), netstr.size());
	buf.fill(netstr.size());

	std::string res;
	UTEST_ASSERT(ns_reader.parse(res));
	UTEST_ASSERT_EQUAL(std::string("Hello, world!"), res);
	UTEST_ASSERT_EQUAL(IO::StreamBuffer::End, stream.get());
}

UTEST_CASE(empty_netstring_test)
{
	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};
	auto ns_reader = Netstring::Reader{stream};
	auto netstr = std::string_view("0:,");
	buf.reserve(netstr.size());
	netstr.copy(static_cast<char *>(buf.wstart()), netstr.size());
	buf.fill(netstr.size());

	std::string res;
	UTEST_ASSERT(ns_reader.parse(res));
	UTEST_ASSERT_EQUAL(std::string(""), res);
	UTEST_ASSERT_EQUAL(IO::StreamBuffer::End, stream.get());
}

UTEST_CASE(missing_length_test)
{
	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};
	auto ns_reader = Netstring::Reader{stream};
	auto netstr = std::string_view(":,");
	buf.reserve(netstr.size());
	netstr.copy(static_cast<char *>(buf.wstart()), netstr.size());
	buf.fill(netstr.size());

	std::string res;
	UTEST_ASSERT_THROW(ns_reader.parse(res), std::runtime_error);
}

UTEST_CASE(missing_comma_test)
{
	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};
	auto ns_reader = Netstring::Reader{stream};
	auto netstr = std::string_view("13:Hello, world!$");
	buf.reserve(netstr.size());
	netstr.copy(static_cast<char *>(buf.wstart()), netstr.size());
	buf.fill(netstr.size());

	std::string res;
	UTEST_ASSERT_THROW(ns_reader.parse(res), std::runtime_error);
}

UTEST_CASE(parse_chunks_test)
{
	auto buf = IO::Buffer{4096};
	auto stream = IO::StreamBuffer{buf};
	auto ns_reader = Netstring::Reader{stream};

	std::string res;

	auto netstr = std::string_view("1");
	buf.reserve(netstr.size());
	netstr.copy(static_cast<char *>(buf.wstart()), netstr.size());
	buf.fill(netstr.size());
	UTEST_ASSERT_EQUAL(false, ns_reader.parse(res));

	netstr = std::string_view("3:Hell");
	buf.reserve(netstr.size());
	netstr.copy(static_cast<char *>(buf.wstart()), netstr.size());
	buf.fill(netstr.size());
	UTEST_ASSERT_EQUAL(false, ns_reader.parse(res));

	netstr = std::string_view("o, world!");
	buf.reserve(netstr.size());
	netstr.copy(static_cast<char *>(buf.wstart()), netstr.size());
	buf.fill(netstr.size());
	UTEST_ASSERT_EQUAL(false, ns_reader.parse(res));

	netstr = std::string_view(",");
	buf.reserve(netstr.size());
	netstr.copy(static_cast<char *>(buf.wstart()), netstr.size());
	buf.fill(netstr.size());
	UTEST_ASSERT_EQUAL(true, ns_reader.parse(res));
	UTEST_ASSERT_EQUAL(std::string("Hello, world!"), res);
	UTEST_ASSERT_EQUAL(IO::StreamBuffer::End, stream.get());
}

}}
