#include <cppunit/extensions/HelperMacros.h>

#include "json/types.h"

namespace unittests {
namespace json_types {

class test : public CppUnit::TestCase {
public:
	test();
	void setUp();
	void tearDown();

private:
	void simple_test();
	void get_if_test();
	void get_test();
	void visit_test();

	CPPUNIT_TEST_SUITE(test);
	CPPUNIT_TEST(simple_test);
	CPPUNIT_TEST(get_if_test);
	CPPUNIT_TEST(get_test);
	CPPUNIT_TEST(visit_test);
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
	auto v = Json::Value{"foo"};
	CPPUNIT_ASSERT(Json::holds_alternative<std::string>(v));

	auto o = Json::Object{
		{"foo", Json::Value{"bar"}}
	};

	auto o2 = Json::make_object({
		{"foo", "bar"},
		{"foo", int64_t(1)},
		{"foo", true},
		{"foo", nullptr}
	});
}

void test::get_if_test()
{
	auto v = Json::Value{};
	if (auto res = Json::get_if<nullptr_t>(&v)) {
		CPPUNIT_ASSERT(nullptr == *res);
	} else {
		CPPUNIT_ASSERT(false);
	}
}

void test::get_test()
{
	auto v = Json::Value{};
	auto res = Json::get<nullptr_t>(v);
	CPPUNIT_ASSERT(nullptr == res);
}

void test::visit_test()
{

}

}}
