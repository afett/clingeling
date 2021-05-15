#include "utest/macros.h"
#include "usb/hid.h"

namespace UTest {
	template <>
	class StringTrait<USB::Hid::Usage::Page> {
	public:
		static std::string to_string(USB::Hid::Usage::Page const& t)
		{
			return std::to_string(uint16_t(t));
		}
	};

	template <>
	class StringTrait<USB::Hid::Usage::Tel> {
	public:
		static std::string to_string(USB::Hid::Usage::Tel const& t)
		{
			return std::to_string(uint16_t(t));
		}
	};
}

namespace unittests {
namespace usb_hid {

UTEST_CASE(simple_test)
{
	{
		USB::Hid::Event ev;
		UTEST_ASSERT(!ev);
	}

	{
		USB::Hid::Event ev{0x000B0020, 0};
		UTEST_ASSERT(ev);
		UTEST_ASSERT_EQUAL(USB::Hid::Usage::Page::Telephony, ev.page());
		UTEST_ASSERT_EQUAL(USB::Hid::Usage::Tel::Hook_Switch, ev.code<USB::Hid::Usage::Tel>());
	}
}

}}
