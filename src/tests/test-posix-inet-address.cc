#include "utest/macros.h"
#include "utest/assertion-traits.h"
#include "posix/inet-address.h"

namespace UTest {

template <>
class StringTrait<Posix::Inet::Address> {
public:
	static std::string to_string(Posix::Inet::Address const& addr)
	{
		return Posix::Inet::to_string(addr);
	}
};

}

namespace unittests {
namespace posix_inet_address {

UTEST_CASE(two_undefined_test)
{
	UTEST_ASSERT(!Posix::Inet::Address{});
	UTEST_ASSERT_EQUAL(Posix::Inet::Address{}, Posix::Inet::Address{});
	auto addr1 = Posix::Inet::Address{};
	auto addr2 = addr1;
	UTEST_ASSERT_EQUAL(Posix::Inet::Address{}, addr1);
	UTEST_ASSERT_EQUAL(Posix::Inet::Address{}, addr2);

	UTEST_ASSERT(!(Posix::Inet::Address{} < Posix::Inet::Address{}));
	UTEST_ASSERT(!(Posix::Inet::Address{} > Posix::Inet::Address{}));
	UTEST_ASSERT(Posix::Inet::Address{} <= Posix::Inet::Address{});
	UTEST_ASSERT(Posix::Inet::Address{} >= Posix::Inet::Address{});
	UTEST_ASSERT(!(Posix::Inet::Address{} != Posix::Inet::Address{}));
}

UTEST_CASE(two_defined_v4_test)
{
	UTEST_ASSERT(Posix::Inet::Address{"192.168.77.1"});
	UTEST_ASSERT_EQUAL(Posix::Inet::Address{"192.168.77.1"}, Posix::Inet::Address{"192.168.77.1"});
	auto addr1 = Posix::Inet::Address{"192.168.77.1"};
	auto addr2 = addr1;
	UTEST_ASSERT_EQUAL(Posix::Inet::Address{"192.168.77.1"}, addr1);
	UTEST_ASSERT_EQUAL(Posix::Inet::Address{"192.168.77.1"}, addr2);

	UTEST_ASSERT(!(Posix::Inet::Address{"192.168.77.1"} < Posix::Inet::Address{"192.168.77.1"}));
	UTEST_ASSERT(!(Posix::Inet::Address{"192.168.77.1"} > Posix::Inet::Address{"192.168.77.1"}));
	UTEST_ASSERT(Posix::Inet::Address{"192.168.77.1"} <= Posix::Inet::Address{"192.168.77.1"});
	UTEST_ASSERT(Posix::Inet::Address{"192.168.77.1"} >= Posix::Inet::Address{"192.168.77.1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"192.168.77.1"} != Posix::Inet::Address{"192.168.77.1"}));
}

UTEST_CASE(two_defined_v6_test)
{
	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT_EQUAL(Posix::Inet::Address{"fd20:a634:9cb5:77::1"}, Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	auto addr1 = Posix::Inet::Address{"fd20:a634:9cb5:77::1"};
	auto addr2 = addr1;
	UTEST_ASSERT_EQUAL(Posix::Inet::Address{"fd20:a634:9cb5:77::1"}, addr1);
	UTEST_ASSERT_EQUAL(Posix::Inet::Address{"fd20:a634:9cb5:77::1"}, addr2);

	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} < Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} > Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));
	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} <= Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} >= Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} != Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));
}

UTEST_CASE(undefined_and_v4_test)
{
	UTEST_ASSERT(!(Posix::Inet::Address{} == Posix::Inet::Address{"192.168.77.1"}));
	UTEST_ASSERT(!(Posix::Inet::Address{"192.168.77.1"} == Posix::Inet::Address{}));

	UTEST_ASSERT(Posix::Inet::Address{} < Posix::Inet::Address{"192.168.77.1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"192.168.77.1"} < Posix::Inet::Address{}));

	UTEST_ASSERT(Posix::Inet::Address{"192.168.77.1"} > Posix::Inet::Address{});
	UTEST_ASSERT(!(Posix::Inet::Address{} > Posix::Inet::Address{"192.168.77.1"}));

	UTEST_ASSERT(Posix::Inet::Address{} <= Posix::Inet::Address{"192.168.77.1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"192.168.77.1"} <= Posix::Inet::Address{}));

	UTEST_ASSERT(Posix::Inet::Address{"192.168.77.1"} >= Posix::Inet::Address{});
	UTEST_ASSERT(!(Posix::Inet::Address{} >= Posix::Inet::Address{"192.168.77.1"}));

	UTEST_ASSERT(Posix::Inet::Address{} != Posix::Inet::Address{"192.168.77.1"});
	UTEST_ASSERT(Posix::Inet::Address{"192.168.77.1"} != Posix::Inet::Address{});
}

UTEST_CASE(undefined_and_v6_test)
{
	UTEST_ASSERT(!(Posix::Inet::Address{} == Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} == Posix::Inet::Address{}));

	UTEST_ASSERT(Posix::Inet::Address{} < Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} < Posix::Inet::Address{}));

	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} > Posix::Inet::Address{});
	UTEST_ASSERT(!(Posix::Inet::Address{} > Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));

	UTEST_ASSERT(Posix::Inet::Address{} <= Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} <= Posix::Inet::Address{}));

	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} >= Posix::Inet::Address{});
	UTEST_ASSERT(!(Posix::Inet::Address{} >= Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));

	UTEST_ASSERT(Posix::Inet::Address{} != Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} != Posix::Inet::Address{});
}

UTEST_CASE(v4_and_v4_test)
{
	UTEST_ASSERT(!(Posix::Inet::Address{"10.0.0.1"} == Posix::Inet::Address{"192.168.77.1"}));
	UTEST_ASSERT(!(Posix::Inet::Address{"192.168.77.1"} == Posix::Inet::Address{"10.0.0.1"}));

	UTEST_ASSERT(Posix::Inet::Address{"10.0.0.1"} < Posix::Inet::Address{"192.168.77.1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"192.168.77.1"} < Posix::Inet::Address{"10.0.0.1"}));

	UTEST_ASSERT(Posix::Inet::Address{"192.168.77.1"} > Posix::Inet::Address{"10.0.0.1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"10.0.0.1"} > Posix::Inet::Address{"192.168.77.1"}));

	UTEST_ASSERT(Posix::Inet::Address{"10.0.0.1"} <= Posix::Inet::Address{"192.168.77.1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"192.168.77.1"} <= Posix::Inet::Address{"10.0.0.1"}));

	UTEST_ASSERT(Posix::Inet::Address{"192.168.77.1"} >= Posix::Inet::Address{"10.0.0.1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"10.0.0.1"} >= Posix::Inet::Address{"192.168.77.1"}));

	UTEST_ASSERT(Posix::Inet::Address{"10.0.0.1"} != Posix::Inet::Address{"192.168.77.1"});
	UTEST_ASSERT(Posix::Inet::Address{"192.168.77.1"} != Posix::Inet::Address{"10.0.0.1"});
}

UTEST_CASE(v6_and_v6_test)
{
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5::1"} == Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} == Posix::Inet::Address{"fd20:a634:9cb5::1"}));

	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5::1"} < Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} < Posix::Inet::Address{"fd20:a634:9cb5::1"}));

	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} > Posix::Inet::Address{"fd20:a634:9cb5::1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5::1"} > Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));

	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5::1"} <= Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} <= Posix::Inet::Address{"fd20:a634:9cb5::1"}));

	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} >= Posix::Inet::Address{"fd20:a634:9cb5::1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5::1"} >= Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));

	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5::1"} != Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} != Posix::Inet::Address{"fd20:a634:9cb5::1"});
}

UTEST_CASE(v4_and_v6_test)
{
	UTEST_ASSERT(!(Posix::Inet::Address{"255.255.255.255"} == Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} == Posix::Inet::Address{"255.255.255.255"}));

	UTEST_ASSERT(Posix::Inet::Address{"255.255.255.255"} < Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} < Posix::Inet::Address{"255.255.255.255"}));

	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} > Posix::Inet::Address{"255.255.255.255"});
	UTEST_ASSERT(!(Posix::Inet::Address{"255.255.255.255"} > Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));

	UTEST_ASSERT(Posix::Inet::Address{"255.255.255.255"} <= Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT(!(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} <= Posix::Inet::Address{"255.255.255.255"}));

	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} >= Posix::Inet::Address{"255.255.255.255"});
	UTEST_ASSERT(!(Posix::Inet::Address{"255.255.255.255"} >= Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));

	UTEST_ASSERT(Posix::Inet::Address{"255.255.255.255"} != Posix::Inet::Address{"fd20:a634:9cb5:77::1"});
	UTEST_ASSERT(Posix::Inet::Address{"fd20:a634:9cb5:77::1"} != Posix::Inet::Address{"255.255.255.255"});
}

UTEST_CASE(to_string_test)
{
	UTEST_ASSERT_EQUAL(std::string{}, to_string(Posix::Inet::Address{}));
	UTEST_ASSERT_EQUAL(std::string{"0.0.0.0"}, to_string(Posix::Inet::Address{"0.0.0.0"}));
	UTEST_ASSERT_EQUAL(std::string{"192.168.77.1"}, to_string(Posix::Inet::Address{"192.168.77.1"}));
	UTEST_ASSERT_EQUAL(std::string{"::"}, to_string(Posix::Inet::Address{"::"}));
	UTEST_ASSERT_EQUAL(std::string{"fd20:a634:9cb5:77::1"}, to_string(Posix::Inet::Address{"fd20:a634:9cb5:77::1"}));
}

UTEST_CASE(size_test)
{
	UTEST_ASSERT_EQUAL(size_t(0), Posix::Inet::Address{}.size());
	UTEST_ASSERT_EQUAL(size_t(4), Posix::Inet::Address{"0.0.0.0"}.size());
	UTEST_ASSERT_EQUAL(size_t(16), Posix::Inet::Address{"::"}.size());
}

}}

