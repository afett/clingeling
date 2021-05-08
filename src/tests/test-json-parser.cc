#include <cppunit/extensions/HelperMacros.h>

#include "json/parser.h"
#include "assertion-traits.h"

namespace unittests {
namespace json_parser {

class test : public CppUnit::TestCase {
public:
	test();
	void setUp();
	void tearDown();

private:
	void empty_object_test();
	void simple_object_test();
	void empty_array_test();
	void simple_array_test();
	void bool_test();
	void null_test();

	CPPUNIT_TEST_SUITE(test);
	CPPUNIT_TEST(empty_object_test);
	CPPUNIT_TEST(simple_object_test);
	CPPUNIT_TEST(empty_array_test);
	CPPUNIT_TEST(simple_array_test);
	CPPUNIT_TEST(bool_test);
	CPPUNIT_TEST(null_test);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(test);

test::test()
{ }

void test::setUp()
{ }

void test::tearDown()
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

void test::bool_test()
{
	std::stringstream is{
		"{"
			"\"foo\": true,"
			"\"bar\": false"
		"}"
	};
	auto o = Json::parse_object(is);
	CPPUNIT_ASSERT(!o.empty());
	CPPUNIT_ASSERT_EQUAL(size_t(2), o.size());
	CPPUNIT_ASSERT(Json::holds_alternative<bool>(o["foo"]));
	CPPUNIT_ASSERT_EQUAL(true, Json::get<bool>(o["foo"]));
	CPPUNIT_ASSERT(Json::holds_alternative<bool>(o["bar"]));
	CPPUNIT_ASSERT_EQUAL(false, Json::get<bool>(o["bar"]));
}

void test::null_test()
{
	std::stringstream is{
		"{"
			"\"foo\": null"
		"}"
	};
	auto o = Json::parse_object(is);
	CPPUNIT_ASSERT(!o.empty());
	CPPUNIT_ASSERT_EQUAL(size_t(1), o.size());
	CPPUNIT_ASSERT(Json::holds_alternative<nullptr_t>(o["foo"]));
	CPPUNIT_ASSERT_EQUAL(nullptr, Json::get<nullptr_t>(o["foo"]));
}

}}
