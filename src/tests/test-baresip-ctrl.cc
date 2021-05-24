#include "utest/macros.h"

#include "baresip/ctrl.h"
#include "baresip/command.h"
#include "io/event-buffer.h"

#include "utest/assertion-traits.h"

namespace UTest {

template <>
class StringTrait<Baresip::Event::Any> {
public:
	static std::string to_string(Baresip::Event::Any const&)
	{
		return "";
	}
};

struct AnyEventEqual {
public:
	AnyEventEqual(Baresip::Event::Any const& ev)
	:
		ev_(ev)
	{ }

	bool operator()(Baresip::Event::Register const& r) const
	{
		if (!std::holds_alternative<Baresip::Event::Register>(ev_)) {
			return false;
		}
		auto l = std::get<Baresip::Event::Register>(ev_);
		return std::tie(l.type, l.accountaor, l.param) ==
			std::tie(r.type, r.accountaor, r.param);
	}

	bool operator()(Baresip::Event::Call const& r) const
	{
		if (!std::holds_alternative<Baresip::Event::Call>(ev_)) {
			return false;
		}
		auto l = std::get<Baresip::Event::Call>(ev_);
		return std::tie(l.type, l.accountaor, l.direction, l.peeruri, l.id, l.param) ==
			std::tie(r.type, r.accountaor, r.direction, r.peeruri, r.id, r.param);
	}
private:
	Baresip::Event::Any ev_;
};

template <>
class EqualTrait<Baresip::Event::Any> {
public:
	static bool equal(Baresip::Event::Any const& l, Baresip::Event::Any const& r)
	{
		return std::visit(AnyEventEqual(l), r);
	}
};

template <>
class StringTrait<Baresip::Command::Response> {
public:
	static std::string to_string(Baresip::Command::Response const&)
	{
		return "";
	}
};

template <>
class EqualTrait<Baresip::Command::Response> {
public:
	static bool equal(Baresip::Command::Response const& l, Baresip::Command::Response const& r)
	{
		return std::tie(l.ok, l.data, l.token) ==
			std::tie(r.ok, r.data, r.token);
	}
};

}

namespace unittests {
namespace baresip_ctrl {

class CtrlTestFixture {
public:
	CtrlTestFixture()
	:
		recvbuf{4096},
		sendbuf{4096},
		ctrl{Baresip::Ctrl::create(recvbuf, sendbuf)}
	{
		recvbuf.on_drain([]() {});
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
};

class EventTestFixture : public CtrlTestFixture {
public:
	EventTestFixture()
	{
		ctrl->on_event([this] (auto const& ev) { have_res = true; res = ev; });
	}

	Baresip::Event::Any res;
};

class CommandTestFixture : public CtrlTestFixture {
public:
	CommandTestFixture()
	{
		ctrl->on_response([this] (auto const& resp) { have_res = true; res = resp; });
	}

	Baresip::Command::Response res;
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

	auto expected = Baresip::Event::Any{Baresip::Event::Register{
		Baresip::Event::Register::Type::Fail,
		"sip:9999-1@asterisk.example.com",
		"401 Unauthorized"}};

	UTEST_ASSERT_EQUAL(expected, res);
}

UTEST_CASE_WITH_FIXTURE(register_ok_test, EventTestFixture)
{
	auto data = std::string{
		"{"
		"\"event\":true,"
		"\"type\":\"REGISTER_OK\","
		"\"class\":\"register\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\","
		"\"param\":\"200 OK\""
		"}"
	};
	send_data(data);

	UTEST_ASSERT(have_res);

	auto expected = Baresip::Event::Any{Baresip::Event::Register{
		Baresip::Event::Register::Type::Ok,
		"sip:9999-1@asterisk.example.com",
		"200 OK"}};

	UTEST_ASSERT_EQUAL(expected, res);
}

UTEST_CASE_WITH_FIXTURE(unregistering_test, EventTestFixture)
{
	auto data = std::string{
		"{"
		"\"event\":true,"
		"\"type\":\"UNREGISTERING\","
		"\"class\":\"register\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\""
		"}"
	};
	send_data(data);

	UTEST_ASSERT(have_res);

	auto expected = Baresip::Event::Any{Baresip::Event::Register{
		Baresip::Event::Register::Type::Unregistering,
		"sip:9999-1@asterisk.example.com",
		""}};

	UTEST_ASSERT_EQUAL(expected, res);
}

UTEST_CASE_WITH_FIXTURE(call_closed_incoming_test, EventTestFixture)
{
	auto data = std::string{
		"{"
		"\"event\":true,"
		"\"type\":\"CALL_CLOSED\","
		"\"class\":\"call\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\","
		"\"direction\":\"incoming\","
		"\"peeruri\":\"sip:7777@192.168.55.1:5060\","
		"\"id\":\"5726089069c0f5d0476be4b315354199@192.168.55.2:5060\","
		"\"param\":\"Connection reset by user\""
		"}"
	};
	send_data(data);

	UTEST_ASSERT(have_res);

	auto expected = Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Closed,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Incoming,
		"sip:7777@192.168.55.1:5060",
		"5726089069c0f5d0476be4b315354199@192.168.55.2:5060",
		"Connection reset by user"}};

	UTEST_ASSERT_EQUAL(expected, res);
}

UTEST_CASE_WITH_FIXTURE(call_closed_outgoing_test, EventTestFixture)
{
	auto data = std::string{
		"{"
		"\"event\":true,"
		"\"type\":\"CALL_CLOSED\","
		"\"class\":\"call\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\","
		"\"direction\":\"outgoing\","
		"\"peeruri\":\"sip:7777@asterisk.example.com;transport=udp\","
		"\"id\":\"6d42101ce49915a3\","
		"\"param\":\"Connection reset by user\""
		"}"
	};
	send_data(data);

	UTEST_ASSERT(have_res);

	auto expected = Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Closed,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Outgoing,
		"sip:7777@asterisk.example.com;transport=udp",
		"6d42101ce49915a3",
		"Connection reset by user"}};

	UTEST_ASSERT_EQUAL(expected, res);
}

UTEST_CASE_WITH_FIXTURE(call_established_incoming_test, EventTestFixture)
{
	auto data = std::string{
		"{"
		"\"event\":true,"
		"\"type\":\"CALL_ESTABLISHED\","
		"\"class\":\"call\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\","
		"\"direction\":\"incoming\","
		"\"peeruri\":\"sip:7777@192.168.55.1:5060\","
		"\"id\":\"5726089069c0f5d0476be4b315354199@192.168.55.2:5060\","
		"\"param\":\"sip:7777@192.168.55.1:5060\""
		"}"
	};
	send_data(data);

	UTEST_ASSERT(have_res);

	auto expected = Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Established,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Incoming,
		"sip:7777@192.168.55.1:5060",
		"5726089069c0f5d0476be4b315354199@192.168.55.2:5060",
		"sip:7777@192.168.55.1:5060"}};

	UTEST_ASSERT_EQUAL(expected, res);
}

UTEST_CASE_WITH_FIXTURE(call_established_outgoing_test, EventTestFixture)
{
	auto data = std::string{
		"{"
		"\"event\":true,"
		"\"type\":\"CALL_ESTABLISHED\","
		"\"class\":\"call\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\","
		"\"direction\":\"outgoing\","
		"\"peeruri\":\"sip:7777@asterisk.example.com;transport=udp\","
		"\"id\":\"6d42101ce49915a3\","
		"\"param\":\"sip:7777@asterisk.example.com;transport=udp\""
		"}"
	};
	send_data(data);

	UTEST_ASSERT(have_res);

	auto expected = Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Established,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Outgoing,
		"sip:7777@asterisk.example.com;transport=udp",
		"6d42101ce49915a3",
		"sip:7777@asterisk.example.com;transport=udp"}};

	UTEST_ASSERT_EQUAL(expected, res);
}

UTEST_CASE_WITH_FIXTURE(call_incoming_test, EventTestFixture)
{
	auto data = std::string{
		"{"
		"\"event\":true,"
		"\"type\":\"CALL_INCOMING\","
		"\"class\":\"call\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\","
		"\"direction\":\"incoming\","
		"\"peeruri\":\"sip:7777@192.168.55.1:5060\","
		"\"id\":\"5726089069c0f5d0476be4b315354199@192.168.55.2:5060\","
		"\"param\":\"sip:7777@192.168.55.1:5060\""
		"}"
	};
	send_data(data);

	UTEST_ASSERT(have_res);

	auto expected = Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Incoming,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Incoming,
		"sip:7777@192.168.55.1:5060",
		"5726089069c0f5d0476be4b315354199@192.168.55.2:5060",
		"sip:7777@192.168.55.1:5060"}};

	UTEST_ASSERT_EQUAL(expected, res);
}

UTEST_CASE_WITH_FIXTURE(call_ringing_test, EventTestFixture)
{
	auto data = std::string{
		"{"
		"\"event\":true,"
		"\"type\":\"CALL_RINGING\","
		"\"class\":\"call\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\","
		"\"direction\":\"outgoing\","
		"\"peeruri\":\"sip:7777@asterisk.example.com;transport=udp\","
		"\"id\":\"6d42101ce49915a3\","
		"\"param\":\"sip:7777@asterisk.example.com;transport=udp\""
		"}"
	};
	send_data(data);

	UTEST_ASSERT(have_res);

	auto expected = Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Ringing,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Outgoing,
		"sip:7777@asterisk.example.com;transport=udp",
		"6d42101ce49915a3",
		"sip:7777@asterisk.example.com;transport=udp"}};

	UTEST_ASSERT_EQUAL(expected, res);
}

UTEST_CASE_WITH_FIXTURE(call_rtcp_test, EventTestFixture)
{
	auto data = std::string{
		"{"
		"\"event\":true,"
		"\"type\":\"CALL_RTCP\","
		"\"class\":\"call\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\","
		"\"direction\":\"incoming\","
		"\"peeruri\":\"sip:7777@192.168.55.1:5060\","
		"\"id\":\"5726089069c0f5d0476be4b315354199@192.168.55.2:5060\","
		"\"param\":\"audio\","
		"\"rtcp_stats\":{\"tx\":{\"sent\":9752,\"lost\":0,\"jit\":1625},\"rx\":{\"sent\":1,\"lost\":0,\"jit\":0},\"rtt\":24995}"
		"}"
	};
	send_data(data);

	UTEST_ASSERT(!have_res);
}

UTEST_CASE_WITH_FIXTURE(vu_report_test, EventTestFixture)
{
	auto data = std::string{
		"{"
		"\"event\":true,"
		"\"type\":\"VU_RX_REPORT\","
		"\"class\":\"VU_REPORT\","
		"\"accountaor\":\"sip:9999-1@asterisk.example.com\","
		"\"direction\":\"incoming\","
		"\"peeruri\":\"sip:7777@192.168.55.1:5060\","
		"\"id\":\"5726089069c0f5d0476be4b315354199@192.168.55.2:5060\","
		"\"param\":\"-60.22\""
		"}"
	};
	send_data(data);

	UTEST_ASSERT(!have_res);
}

UTEST_CASE_WITH_FIXTURE(response_test, CommandTestFixture)
{
	auto data = std::string{
		"{"
		"\"response\":true,"
		"\"ok\":true,"
		"\"data\":\"\","
		"\"token\":\"01AAFE489\""
		"}"
	};
	send_data(data);

	UTEST_ASSERT(have_res);

	auto expected = Baresip::Command::Response{
		true,
		"",
		"01AAFE489"};

	UTEST_ASSERT_EQUAL(expected, res);
}

/*

{"event":true,"type":"EXIT","class":"application"},
{"event":true,"type":"SHUTDOWN","class":"application","accountaor":"sip:9999-1@asterisk.example.com"}

*/

}}
