#include "utest/macros.h"

#include "baresip/ctrl.h"
#include "io/event-buffer.h"

#include "utest/assertion-traits.h"

namespace UTest {

template <>
class StringTrait<Baresip::Event> {
public:
	static std::string to_string(Baresip::Event const&)
	{
		return "";
	}
};

template <>
class EqualTrait<Baresip::Event> {
public:
	static bool equal(Baresip::Event const& l, Baresip::Event const& r)
	{
		return std::tie(l.type, l.klass, l.accountaor, l.param) ==
			std::tie(r.type, r.klass, r.accountaor, r.param);
	}
};

}

namespace unittests {
namespace baresip_ctrl {

class EventTestFixture {
public:
	EventTestFixture()
	:
		recvbuf{4096},
		sendbuf{4096},
		ctrl{Baresip::Ctrl::create(recvbuf, sendbuf)}
	{
		recvbuf.on_drain([]() {});
		ctrl->on_event([this] (auto const& ev) { have_res = true; res = ev; });
	}

	void send_data(std::string_view const& data)
	{
		recvbuf.reserve(data.size());
		data.copy(static_cast<char *>(recvbuf.wstart()), data.size());
		recvbuf.fill(data.size());
	}

	IO::EventBuffer recvbuf;
	IO::EventBuffer sendbuf;
	std::unique_ptr<Baresip::Ctrl> ctrl;
	bool have_res = false;
	Baresip::Event res;
};

UTEST_CASE_WITH_FIXTURE(register_fail_test, EventTestFixture)
{
	auto data = std::string_view{
		"130:{\""
		"event\":true,"
		"\"type\":\"REGISTER_FAIL\","
		"\"class\":\"register\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\","
		"\"param\":\"401 Unauthorized\"},"};
	send_data(data);

	UTEST_ASSERT(have_res);

	auto expected = Baresip::Event{
		Baresip::Event::Type::RegisterFail,
		Baresip::Event::Class::Register,
		"sip:9999-1@asterisk.example.com",
		"401 Unauthorized"};

	UTEST_ASSERT_EQUAL(expected, res);
}

}}
