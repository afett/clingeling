#include <cppunit/extensions/HelperMacros.h>

#include "json/parser.h"

namespace unittests {
namespace json_parser {

class test : public CppUnit::TestCase {
public:
	test();
	void setUp();
	void tearDown();

private:
	void simple_test();
	void empty_object_test();
	void simple_object_test();
	void empty_array_test();
	void simple_array_test();

	CPPUNIT_TEST_SUITE(test);
	CPPUNIT_TEST(simple_test);
	CPPUNIT_TEST(empty_object_test);
	CPPUNIT_TEST(simple_object_test);
	CPPUNIT_TEST(empty_array_test);
	CPPUNIT_TEST(simple_array_test);
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
{ }

void test::empty_object_test()
{
	{
		std::stringstream is{"{}"};
		auto o = Json::parse_object(is);
		CPPUNIT_ASSERT(o.empty());
	}

	{
		std::stringstream is{" {}"};
		auto o = Json::parse_object(is);
		CPPUNIT_ASSERT(o.empty());
	}

	{
		std::stringstream is{"{} "};
		auto o = Json::parse_object(is);
		CPPUNIT_ASSERT(o.empty());
	}

	{
		std::stringstream is{"{ }"};
		auto o = Json::parse_object(is);
		CPPUNIT_ASSERT(o.empty());
	}

	{
		std::stringstream is{" { } "};
		auto o = Json::parse_object(is);
		CPPUNIT_ASSERT(o.empty());
	}

	{
		std::stringstream is{"    {        }     "};
		auto o = Json::parse_object(is);
		CPPUNIT_ASSERT(o.empty());
	}
}

void test::simple_object_test()
{
	std::stringstream is{
		"{"
			"\"foo\": \"bar\""
		"}"
	};
	auto o = Json::parse_object(is);
	CPPUNIT_ASSERT(!o.empty());
	CPPUNIT_ASSERT_EQUAL(size_t(1), o.size());
	CPPUNIT_ASSERT(Json::holds_alternative<std::string>(o["foo"]));
	CPPUNIT_ASSERT_EQUAL(std::string{"bar"}, Json::get<std::string>(o["foo"]));
}

void test::empty_array_test()
{
	{
		std::stringstream is{"[]"};
		auto a = Json::parse_array(is);
		CPPUNIT_ASSERT(a.empty());
	}

	{
		std::stringstream is{" []"};
		auto a = Json::parse_array(is);
		CPPUNIT_ASSERT(a.empty());
	}

	{
		std::stringstream is{"[] "};
		auto a = Json::parse_array(is);
		CPPUNIT_ASSERT(a.empty());
	}

	{
		std::stringstream is{"[ ]"};
		auto a = Json::parse_array(is);
		CPPUNIT_ASSERT(a.empty());
	}

	{
		std::stringstream is{" [ ] "};
		auto a = Json::parse_array(is);
		CPPUNIT_ASSERT(a.empty());
	}

	{
		std::stringstream is{"    [        ]     "};
		auto o = Json::parse_array(is);
		CPPUNIT_ASSERT(o.empty());
	}
}

void test::simple_array_test()
{
	std::stringstream is{
		"["
			"\"bar\""
		"]"
	};
	auto a = Json::parse_array(is);
	CPPUNIT_ASSERT(!a.empty());
	CPPUNIT_ASSERT_EQUAL(size_t(1), a.size());
	CPPUNIT_ASSERT(Json::holds_alternative<std::string>(a[0]));
	CPPUNIT_ASSERT_EQUAL(std::string{"bar"}, Json::get<std::string>(a[0]));
}

}}
