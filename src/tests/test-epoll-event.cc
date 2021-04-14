#include <cppunit/extensions/HelperMacros.h>

#include "epoll/event.h"

namespace unittests {
namespace EpollEvent {

class test : public CppUnit::TestCase {
public:
	test();
	void setUp();
	void tearDown();

private:
	void default_ctor_test();
	void operator_bool_test();
	void operator_bit_and_test();
	void operator_bit_or_test();

	CPPUNIT_TEST_SUITE(test);
	CPPUNIT_TEST(default_ctor_test);
	CPPUNIT_TEST(operator_bool_test);
	CPPUNIT_TEST(operator_bit_and_test);
	CPPUNIT_TEST(operator_bit_or_test);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(test);

test::test()
{ }

void test::setUp()
{ }

void test::tearDown()
{ }

void test::default_ctor_test()
{
	EPoll::Event ev;
	CPPUNIT_ASSERT(!ev);
}

void test::operator_bool_test()
{
	{
		EPoll::Event ev;
		CPPUNIT_ASSERT(!ev);
	}

	{
		EPoll::Event ev{EPoll::Event::Type::In};
		CPPUNIT_ASSERT(ev);
	}
}

void test::operator_bit_and_test()
{
	{
		EPoll::Event ev;
		CPPUNIT_ASSERT(!(ev & EPoll::Event::Type::In));
	}

	{
		EPoll::Event ev{EPoll::Event::Type::In};
		CPPUNIT_ASSERT(ev & EPoll::Event::Type::In);
	}
}

void test::operator_bit_or_test()
{
	{
		EPoll::Event ev;
		CPPUNIT_ASSERT(!(ev & EPoll::Event::Type::In));
	}

	{
		EPoll::Event ev{EPoll::Event::Type::In};
		CPPUNIT_ASSERT(ev & EPoll::Event::Type::In);
	}
}

}}
