#include "utest/macros.h"

#include "epoll/ctrl.h"

namespace unittests {
namespace EpollEvent {

UTEST_CASE(default_ctor_test)
{
	EPoll::Events ev;
	UTEST_ASSERT(!ev);
}

UTEST_CASE(operator_bool_test)
{
	{
		EPoll::Events ev;
		UTEST_ASSERT(!ev);
	}

	{
		EPoll::Events ev{EPoll::Event::In};
		UTEST_ASSERT(ev);
	}
}

UTEST_CASE(operator_bit_and_test)
{
	{
		EPoll::Events ev;
		UTEST_ASSERT(!(ev & EPoll::Event::In));
	}

	{
		EPoll::Events ev{EPoll::Event::In};
		UTEST_ASSERT(ev & EPoll::Event::In);
	}
}

UTEST_CASE(operator_bit_or_test)
{
	{
		EPoll::Events ev;
		UTEST_ASSERT(!(ev & EPoll::Event::In));
	}

	{
		EPoll::Events ev{EPoll::Event::In};
		UTEST_ASSERT(ev & EPoll::Event::In);
	}
}

UTEST_CASE(clingeling_test)
{
	EPoll::Events ev{EPoll::Event::In};
	ev = EPoll::Event::Out;

	if (ev & EPoll::Event::Hup) {
		UTEST_ASSERT(false);
	}
	if (ev & EPoll::Event::In) {
		UTEST_ASSERT(false);
	}
	UTEST_ASSERT(ev & EPoll::Event::Out);
	UTEST_ASSERT(!(ev & EPoll::Event::In));
}

}}
