#include "utest/macros.h"
#include "baresip/model.h"

namespace UTest {

template <>
class StringTrait<Baresip::Model::Registration> {
public:
	static std::string to_string(Baresip::Model::Registration const&)
	{
		return "";
	}
};

template <>
class StringTrait<Baresip::Call::State> {
public:
	static std::string to_string(Baresip::Call::State const&)
	{
		return "";
	}
};

template <>
class StringTrait<Baresip::Call::Direction> {
public:
	static std::string to_string(Baresip::Call::Direction const&)
	{
		return "";
	}
};

template <>
class StringTrait<Baresip::Call::Id> {
public:
	static std::string to_string(Baresip::Call::Id const& id)
	{
		return to_string(id);
	}
};

}

namespace unittests {
namespace baresip_model {

class Fixture {
public:
	Fixture()
	:
		model(Baresip::Model::create())
	{
		connect(model->on_event(), on_event);
	}

	Signal<void(Baresip::Event::Any const&)> on_event;
	std::unique_ptr<Baresip::Model> model;
};

UTEST_CASE_WITH_FIXTURE(default_ctor_test, Fixture)
{
	UTEST_ASSERT_EQUAL(Baresip::Model::Registration::Unknown, model->registration());
	UTEST_ASSERT(model->calls().empty());
}

UTEST_CASE_WITH_FIXTURE(incoming_call_test, Fixture)
{
	UTEST_ASSERT_EQUAL(Baresip::Model::Registration::Unknown, model->registration());
	UTEST_ASSERT(model->calls().empty());

	std::shared_ptr<Baresip::Call> call;
	model->on_call().connect([&call](auto const& c) { call = c; });

	// incomming call
	on_event(Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Incoming,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Incoming,
		"sip:7777@192.168.55.1:5060",
		"5726089069c0f5d0476be4b315354199@192.168.55.2:5060",
		"sip:7777@192.168.55.1:5060"}});

	UTEST_ASSERT(call);
	UTEST_ASSERT_EQUAL(Baresip::Call::Id{"5726089069c0f5d0476be4b315354199@192.168.55.2:5060"}, call->id());
	UTEST_ASSERT_EQUAL(Baresip::Call::State::Incoming, call->state());
	UTEST_ASSERT_EQUAL(Baresip::Call::Direction::Incoming, call->direction());
	UTEST_ASSERT_EQUAL("sip:9999-1@asterisk.example.com", call->accountaor());
	UTEST_ASSERT_EQUAL("sip:7777@192.168.55.1:5060", call->peeruri());

	auto calls = model->calls();
	UTEST_ASSERT_EQUAL(size_t(1), calls.size());
	UTEST_ASSERT_EQUAL(call, calls[0]);

	Baresip::Call::State call_state = Baresip::Call::State::Closed;
	call->on_state_change().connect([&call_state](auto state){ call_state = state; });

	// incomming call accepted
	on_event(Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Established,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Incoming,
		"sip:7777@192.168.55.1:5060",
		"5726089069c0f5d0476be4b315354199@192.168.55.2:5060",
		"sip:7777@192.168.55.1:5060"}});

	UTEST_ASSERT_EQUAL(Baresip::Call::State::Established, call_state);
	UTEST_ASSERT_EQUAL(Baresip::Call::State::Established, call->state());

	// incomming call closed
	on_event(Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Closed,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Incoming,
		"sip:7777@192.168.55.1:5060",
		"5726089069c0f5d0476be4b315354199@192.168.55.2:5060",
		"Connection reset by user"}});

	UTEST_ASSERT_EQUAL(Baresip::Call::State::Closed, call_state);
	UTEST_ASSERT_EQUAL(Baresip::Call::State::Closed, call->state());
}

UTEST_CASE_WITH_FIXTURE(outgoing_call_test, Fixture)
{
	UTEST_ASSERT_EQUAL(Baresip::Model::Registration::Unknown, model->registration());
	UTEST_ASSERT(model->calls().empty());

	std::shared_ptr<Baresip::Call> call;
	model->on_call().connect([&call](auto const& c) { call = c; });

	// outgoing
	on_event(Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Ringing,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Outgoing,
		"sip:7777@asterisk.example.com;transport=udp",
		"6d42101ce49915a3",
		"sip:7777@asterisk.example.com;transport=udp"}});

	UTEST_ASSERT(call);
	UTEST_ASSERT_EQUAL(Baresip::Call::Id{"6d42101ce49915a3"}, call->id());
	UTEST_ASSERT_EQUAL(Baresip::Call::State::Ringing, call->state());
	UTEST_ASSERT_EQUAL(Baresip::Call::Direction::Outgoing, call->direction());
	UTEST_ASSERT_EQUAL("sip:9999-1@asterisk.example.com", call->accountaor());
	UTEST_ASSERT_EQUAL("sip:7777@asterisk.example.com;transport=udp", call->peeruri());

	auto calls = model->calls();
	UTEST_ASSERT_EQUAL(size_t(1), calls.size());
	UTEST_ASSERT_EQUAL(call, calls[0]);

	Baresip::Call::State call_state = Baresip::Call::State::Closed;
	call->on_state_change().connect([&call_state](auto state){ call_state = state; });

	// outgoing call accepted
	on_event(Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Established,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Outgoing,
		"sip:7777@asterisk.example.com;transport=udp",
		"6d42101ce49915a3",
		"sip:7777@asterisk.example.com;transport=udp"}});

	UTEST_ASSERT_EQUAL(Baresip::Call::State::Established, call_state);
	UTEST_ASSERT_EQUAL(Baresip::Call::State::Established, call->state());

	// incomming call closed
	on_event(Baresip::Event::Any{Baresip::Event::Call{
		Baresip::Event::Call::Type::Closed,
		"sip:9999-1@asterisk.example.com",
		Baresip::Event::Call::Direction::Outgoing,
		"sip:7777@asterisk.example.com;transport=udp",
		"6d42101ce49915a3",
		"Connection reset by user"}});

	UTEST_ASSERT_EQUAL(Baresip::Call::State::Closed, call_state);
	UTEST_ASSERT_EQUAL(Baresip::Call::State::Closed, call->state());
}

UTEST_CASE_WITH_FIXTURE(registration_ok_test, Fixture)
{
	UTEST_ASSERT_EQUAL(Baresip::Model::Registration::Unknown, model->registration());

	on_event(Baresip::Event::Any{Baresip::Event::Register{
		Baresip::Event::Register::Type::Ok,
		"sip:9999-1@asterisk.example.com",
		"200 OK"}});

	UTEST_ASSERT_EQUAL(Baresip::Model::Registration::Ok, model->registration());
}

UTEST_CASE_WITH_FIXTURE(registration_fail_test, Fixture)
{
	UTEST_ASSERT_EQUAL(Baresip::Model::Registration::Unknown, model->registration());

	on_event(Baresip::Event::Any{Baresip::Event::Register{
		Baresip::Event::Register::Type::Fail,
		"sip:9999-1@asterisk.example.com",
		"401 Unauthorized"}});

	UTEST_ASSERT_EQUAL(Baresip::Model::Registration::Fail, model->registration());
}

UTEST_CASE_WITH_FIXTURE(registration_unregistering_test, Fixture)
{
	// move to registration ok first
	on_event(Baresip::Event::Any{Baresip::Event::Register{
		Baresip::Event::Register::Type::Ok,
		"sip:9999-1@asterisk.example.com",
		"200 OK"}});
	UTEST_ASSERT_EQUAL(Baresip::Model::Registration::Ok, model->registration());

	// unregistering
	on_event(Baresip::Event::Any{Baresip::Event::Register{
		Baresip::Event::Register::Type::Unregistering,
		"sip:9999-1@asterisk.example.com",
		""}});
	UTEST_ASSERT_EQUAL(Baresip::Model::Registration::Unknown, model->registration());
}

UTEST_CASE(call_id_test)
{
	UTEST_ASSERT(!Baresip::Call::Id{});
	UTEST_ASSERT(!Baresip::Call::Id{""});
	UTEST_ASSERT(Baresip::Call::Id{"abc"});

	UTEST_ASSERT(Baresip::Call::Id{} < Baresip::Call::Id{"a"});
	UTEST_ASSERT(Baresip::Call::Id{"a"} < Baresip::Call::Id{"b"});
	UTEST_ASSERT(Baresip::Call::Id{"abc"} < Baresip::Call::Id{"b"});

	UTEST_ASSERT(Baresip::Call::Id{"a"} > Baresip::Call::Id{});
	UTEST_ASSERT(Baresip::Call::Id{"b"} > Baresip::Call::Id{"a"});
	UTEST_ASSERT(Baresip::Call::Id{"b"} > Baresip::Call::Id{"abc"});

	UTEST_ASSERT(Baresip::Call::Id{} == Baresip::Call::Id{});
	UTEST_ASSERT(Baresip::Call::Id{"a"} == Baresip::Call::Id{"a"});

	UTEST_ASSERT(Baresip::Call::Id{} != Baresip::Call::Id{"a"});
	UTEST_ASSERT(Baresip::Call::Id{"b"} != Baresip::Call::Id{"a"});
}

}}
