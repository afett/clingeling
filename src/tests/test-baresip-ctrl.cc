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

	void send_data(std::string const& str)
	{
		auto netstr = std::to_string(str.size()) + ':' + str + ',';
		recvbuf.reserve(netstr.size());
		netstr.copy(static_cast<char *>(recvbuf.wstart()), netstr.size());
		recvbuf.fill(netstr.size());
	}

	IO::EventBuffer recvbuf;
	IO::EventBuffer sendbuf;
	std::unique_ptr<Baresip::Ctrl> ctrl;
	bool have_res = false;
	Baresip::Event res;
};

UTEST_CASE_WITH_FIXTURE(register_fail_test, EventTestFixture)
{
	auto data = std::string{
		"{"
		"\"event\":true,"
		"\"type\":\"REGISTER_FAIL\","
		"\"class\":\"register\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\","
		"\"param\":\"401 Unauthorized\""
		"}"
	};
	send_data(data);

	UTEST_ASSERT(have_res);

	auto expected = Baresip::Event{
		Baresip::Event::Type::RegisterFail,
		Baresip::Event::Class::Register,
		"sip:9999-1@asterisk.example.com",
		"401 Unauthorized"};

	UTEST_ASSERT_EQUAL(expected, res);
}

/*

{"event":true,"type":"CALL_CLOSED","class":"call","accountaor":"sip:9999-1@asterisk.example.com","direction":"incoming","peeruri":"sip:7777@192.168.55.1:5060","id":"5726089069c0f5d0476be4b315354199@192.168.55.2:5060","param":"Connection reset by user"}
{"event":true,"type":"CALL_CLOSED","class":"call","accountaor":"sip:9999-1@asterisk.example.com","direction":"outgoing","peeruri":"sip:7777@asterisk.example.com;transport=udp","id":"6d42101ce49915a3","param":"Connection reset by user"}
{"event":true,"type":"CALL_CLOSED","class":"call","accountaor":"sip:9999-1@asterisk.example.com","direction":"outgoing","peeruri":"sip:7777@asterisk.example.com;transport=udp","id":"9e62dfd55fe6aa7d","param":"No such device"}
{"event":true,"type":"CALL_ESTABLISHED","class":"call","accountaor":"sip:9999-1@asterisk.example.com","direction":"incoming","peeruri":"sip:7777@192.168.55.1:5060","id":"5726089069c0f5d0476be4b315354199@192.168.55.2:5060","param":"sip:7777@192.168.55.1:5060"}
{"event":true,"type":"CALL_ESTABLISHED","class":"call","accountaor":"sip:9999-1@asterisk.example.com","direction":"outgoing","peeruri":"sip:7777@asterisk.example.com;transport=udp","id":"6d42101ce49915a3","param":"sip:7777@asterisk.example.com;transport=udp"}
{"event":true,"type":"CALL_INCOMING","class":"call","accountaor":"sip:9999-1@asterisk.example.com","direction":"incoming","peeruri":"sip:7777@192.168.55.1:5060","id":"5726089069c0f5d0476be4b315354199@192.168.55.2:5060","param":"sip:7777@192.168.55.1:5060"}
{"event":true,"type":"CALL_RINGING","class":"call","accountaor":"sip:9999-1@asterisk.example.com","direction":"outgoing","peeruri":"sip:7777@asterisk.example.com;transport=udp","id":"6d42101ce49915a3","param":"sip:7777@asterisk.example.com;transport=udp"}
{"event":true,"type":"CALL_RTCP","class":"call","accountaor":"sip:9999-1@asterisk.example.com","direction":"incoming","peeruri":"sip:7777@192.168.55.1:5060","id":"5726089069c0f5d0476be4b315354199@192.168.55.2:5060","param":"audio","rtcp_stats":{"tx":{"sent":9752,"lost":0,"jit":1625},"rx":{"sent":1,"lost":0,"jit":0},"rtt":24995}}
{"event":true,"type":"CALL_RTCP","class":"call","accountaor":"sip:9999-1@asterisk.example.com","direction":"outgoing","peeruri":"sip:7777@asterisk.example.com;transport=udp","id":"6d42101ce49915a3","param":"audio","rtcp_stats":{"tx":{"sent":10002,"lost":0,"jit":1500},"rx":{"sent":0,"lost":0,"jit":0},"rtt":25284}}
{"event":true,"type":"EXIT","class":"application"},
{"event":true,"type":"REGISTER_FAIL","class":"register","accountaor":"sip:9999-1@asterisk.example.com","param":"401 Unauthorized"}
{"event":true,"type":"REGISTER_FAIL","class":"register","accountaor":"sip:9999-1@asterisk.example.com","param":"Connection timed out"}
{"event":true,"type":"REGISTER_OK","class":"register","accountaor":"sip:9999-1@asterisk.example.com","param":"200 OK"}
{"event":true,"type":"SHUTDOWN","class":"application","accountaor":"sip:9999-1@asterisk.example.com"}
{"event":true,"type":"UNREGISTERING","class":"register","accountaor":"sip:9999-1@asterisk.example.com"}
{"event":true,"type":"VU_RX_REPORT","class":"VU_REPORT","accountaor":"sip:9999-1@asterisk.example.com","direction":"incoming","peeruri":"sip:7777@192.168.55.1:5060","id":"5726089069c0f5d0476be4b315354199@192.168.55.2:5060","param":"-60.22"}
{"event":true,"type":"VU_RX_REPORT","class":"VU_REPORT","accountaor":"sip:9999-1@asterisk.example.com","direction":"outgoing","peeruri":"sip:7777@asterisk.example.com;transport=udp","id":"6d42101ce49915a3","param":"-13.58"}
{"event":true,"type":"VU_TX_REPORT","class":"VU_REPORT","accountaor":"sip:9999-1@asterisk.example.com","direction":"incoming","peeruri":"sip:7777@192.168.55.1:5060","id":"5726089069c0f5d0476be4b315354199@192.168.55.2:5060","param":"-13.96"}
{"event":true,"type":"VU_TX_REPORT","class":"VU_REPORT","accountaor":"sip:9999-1@asterisk.example.com","direction":"outgoing","peeruri":"sip:7777@asterisk.example.com;transport=udp","id":"6d42101ce49915a3","param":"-14.54"}

*/

}}
