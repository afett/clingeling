#include "utest/macros.h"
#include "posix/socket-address.h"
#include "posix/inet-address.h"

#include "utest/assertion-traits.h"

namespace UTest {

template <>
struct StringTrait<Posix::SocketAddress>
{
	static std::string to_string(Posix::SocketAddress const& addr)
	{
		return to_string(addr);
	}
};

}

namespace unittests {
namespace posix_socket_address {

UTEST_CASE(simple_test)
{
	UTEST_ASSERT(!Posix::SocketAddress{});
	UTEST_ASSERT(Posix::SocketAddress{} == Posix::SocketAddress{});
	UTEST_ASSERT(!(Posix::SocketAddress{} != Posix::SocketAddress{}));
	UTEST_ASSERT(!(Posix::SocketAddress{} < Posix::SocketAddress{}));
	UTEST_ASSERT(!(Posix::SocketAddress{} > Posix::SocketAddress{}));
	UTEST_ASSERT(Posix::SocketAddress{} <= Posix::SocketAddress{});
	UTEST_ASSERT(Posix::SocketAddress{} >= Posix::SocketAddress{});

	auto addr1 = Posix::SocketAddress{Posix::Inet::Address{"192.168.77.1"}, 1234};
	auto addr2 = Posix::SocketAddress{Posix::Inet::Address{"192.168.77.1"}, 1234};
	UTEST_ASSERT(addr1);
	UTEST_ASSERT(addr1 == addr2);
	UTEST_ASSERT(!(addr1 != addr2));
	UTEST_ASSERT(!(addr1 < addr2));
	UTEST_ASSERT(!(addr1 > addr2));
	UTEST_ASSERT(addr1 <= addr2);
	UTEST_ASSERT(addr1 >= addr2);

	UTEST_ASSERT(!(addr1 == Posix::SocketAddress{}));
	UTEST_ASSERT(addr1 != Posix::SocketAddress{});
	UTEST_ASSERT(!(addr1 < Posix::SocketAddress{}));
	UTEST_ASSERT(addr1 > Posix::SocketAddress{});

	auto addr3 = Posix::SocketAddress{Posix::Inet::Address{"192.168.77.1"}, 4567};
	UTEST_ASSERT(addr3 > addr1);
}

UTEST_CASE(to_string_test)
{
	UTEST_ASSERT_EQUAL(std::string{}, to_string(Posix::SocketAddress{}));
	UTEST_ASSERT_EQUAL(std::string{"192.168.77.1:1234"}, to_string(Posix::SocketAddress{Posix::Inet::Address{"192.168.77.1"}, 1234}));
	UTEST_ASSERT_EQUAL(std::string{"[fd20:a634:9cb5:77::1]:5678"}, to_string(Posix::SocketAddress{Posix::Inet::Address{"fd20:a634:9cb5:77::1"}, 5678}));
}

}}
