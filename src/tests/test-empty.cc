#include "utest/macros.h"

namespace unittests {
namespace empty {

UTEST_CASE(simple_test)
{
	UTEST_ASSERT(true);
}

class Fixture {
public:
	int value = 42;
};

UTEST_CASE_WITH_FIXTURE(simple_fixture_test, Fixture)
{
	UTEST_ASSERT_EQUAL(42, value);
}

}}
