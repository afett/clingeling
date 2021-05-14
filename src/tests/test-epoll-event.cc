#include "utest/macros.h"

#include "epoll/event.h"

namespace unittests {
namespace EpollEvent {

UTEST_CASE(default_ctor_test)
{
	EPoll::Event ev;
	UTEST_ASSERT(!ev);
}

UTEST_CASE(operator_bool_test)
{
	{
		EPoll::Event ev;
		UTEST_ASSERT(!ev);
	}

	{
		EPoll::Event ev{EPoll::Event::Type::In};
		UTEST_ASSERT(ev);
	}
}

UTEST_CASE(operator_bit_and_test)
{
	{
		EPoll::Event ev;
		UTEST_ASSERT(!(ev & EPoll::Event::Type::In));
	}

	{
		EPoll::Event ev{EPoll::Event::Type::In};
		UTEST_ASSERT(ev & EPoll::Event::Type::In);
	}
}

UTEST_CASE(operator_bit_or_test)
{
	{
		EPoll::Event ev;
		UTEST_ASSERT(!(ev & EPoll::Event::Type::In));
	}

	{
		EPoll::Event ev{EPoll::Event::Type::In};
		UTEST_ASSERT(ev & EPoll::Event::Type::In);
	}
}

UTEST_CASE(clingeling_test)
{
	EPoll::Event ev{EPoll::Event::Type::In};
	ev = EPoll::Event::Type::Out;

	if (ev & EPoll::Event::Type::Hup) {
		UTEST_ASSERT(false);
	}
	if (ev & EPoll::Event::Type::In) {
		UTEST_ASSERT(false);
	}
	UTEST_ASSERT(ev & EPoll::Event::Type::Out);
	UTEST_ASSERT(!(ev & EPoll::Event::Type::In));
}

}}
