#include <cppunit/extensions/HelperMacros.h>

namespace unittests {
namespace empty {

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
{ }

}}
