#include <cppunit/extensions/HelperMacros.h>

#include "json/serializer.h"

namespace unittests {
namespace json_serializer {

class test : public CppUnit::TestCase {
public:
	test();
	void setUp();
	void tearDown();

private:
	void serialize_object_test();
	void serialize_array_test();

	CPPUNIT_TEST_SUITE(test);
	CPPUNIT_TEST(serialize_object_test);
	CPPUNIT_TEST(serialize_array_test);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(test);

test::test()
{ }

void test::setUp()
{ }

void test::tearDown()
{ }

void test::serialize_object_test()
{
	auto o = Json::make_object({
		{"null", nullptr},
		{"true", true},
		{"false", false},
		{"uint64_t", uint64_t(42)},
		{"int64_t", int64_t(-42)},
		{"string", "foobar"},
		{"array", Json::make_array({})},
		{"object", Json::make_object({})},
	});

	auto ref = std::string{
		"{"
		"\"array\": [], "
		"\"false\": false, "
		"\"int64_t\": -42, "
		"\"null\": null, "
		"\"object\": {}, "
		"\"string\": \"foobar\", "
		"\"true\": true, "
		"\"uint64_t\": 42"
		"}"
	};

	CPPUNIT_ASSERT_EQUAL(ref, Json::to_string(o));
}

void test::serialize_array_test()
{
	auto o = Json::make_array({
		nullptr,
		true,
		false,
		uint64_t(42),
		int64_t(-42),
		"foobar",
		Json::make_array({}),
		Json::make_object({}),
	});

	auto ref = std::string{
		"["
		"null, "
		"true, "
		"false, "
		"42, "
		"-42, "
		"\"foobar\", "
		"[], "
		"{}"
		"]"
	};

	CPPUNIT_ASSERT_EQUAL(ref, Json::to_string(o));
}

}}
