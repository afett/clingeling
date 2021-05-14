#include "utest/macros.h"

#include "fmt.h"

namespace unittests {
namespace format {

UTEST_CASE(simple_test)
{
	{
		auto str = Fmt::format("");
		UTEST_ASSERT_EQUAL(std::string(), str);
	}

	{
		auto str = Fmt::format("%");
		UTEST_ASSERT_EQUAL(std::string("%"), str);
	}

	{
		// auto str = Fmt::format("%%");
		// UTEST_ASSERT_EQUAL(std::string("%"), str);
	}

	{
		// auto str = Fmt::format("%%%");
		// UTEST_ASSERT_EQUAL(std::string("%%"), str);
	}

	{
		// auto str = Fmt::format("%%%%");
		// UTEST_ASSERT_EQUAL(std::string("%%"), str);
	}

	{
		auto str = Fmt::format("foo bar %");
		UTEST_ASSERT_EQUAL(std::string("foo bar %"), str);
	}

	{
		auto str = Fmt::format("foo %s bar");
		UTEST_ASSERT_EQUAL(std::string("foo %s bar"), str);
	}

	{
		auto str = Fmt::format("%s");
		UTEST_ASSERT_EQUAL(std::string("%s"), str);
	}

	{
		auto str = Fmt::format("%s", 1);
		UTEST_ASSERT_EQUAL(std::string("1"), str);
	}

	{
		auto str = Fmt::format("%x", 10);
		UTEST_ASSERT_EQUAL(std::string("0xa"), str);
	}

	{
		auto str = Fmt::format("%x %x", 10, 11);
		UTEST_ASSERT_EQUAL(std::string("0xa 0xb"), str);
	}

	{
		auto str = Fmt::format("%x %s", 10, 11);
		UTEST_ASSERT_EQUAL(std::string("0xa 11"), str);
	}
}

}}
