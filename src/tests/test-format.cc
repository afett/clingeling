#include <cppunit/extensions/HelperMacros.h>

#include "fmt.h"

namespace unittests {
namespace format {

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
	{
		auto str = Fmt::format("");
		CPPUNIT_ASSERT_EQUAL(std::string(), str);
	}

	{
		auto str = Fmt::format("%");
		CPPUNIT_ASSERT_EQUAL(std::string("%"), str);
	}

	{
		// auto str = Fmt::format("%%");
		// CPPUNIT_ASSERT_EQUAL(std::string("%"), str);
	}

	{
		// auto str = Fmt::format("%%%");
		// CPPUNIT_ASSERT_EQUAL(std::string("%%"), str);
	}

	{
		// auto str = Fmt::format("%%%%");
		// CPPUNIT_ASSERT_EQUAL(std::string("%%"), str);
	}

	{
		auto str = Fmt::format("foo bar %");
		CPPUNIT_ASSERT_EQUAL(std::string("foo bar %"), str);
	}

	{
		auto str = Fmt::format("foo %s bar");
		CPPUNIT_ASSERT_EQUAL(std::string("foo %s bar"), str);
	}

	{
		auto str = Fmt::format("%s");
		CPPUNIT_ASSERT_EQUAL(std::string("%s"), str);
	}

	{
		auto str = Fmt::format("%s", 1);
		CPPUNIT_ASSERT_EQUAL(std::string("1"), str);
	}

	{
		auto str = Fmt::format("%x", 10);
		CPPUNIT_ASSERT_EQUAL(std::string("0xa"), str);
	}

	{
		auto str = Fmt::format("%x %x", 10, 11);
		CPPUNIT_ASSERT_EQUAL(std::string("0xa 0xb"), str);
	}

	{
		auto str = Fmt::format("%x %s", 10, 11);
		CPPUNIT_ASSERT_EQUAL(std::string("0xa 11"), str);
	}
}

}}
