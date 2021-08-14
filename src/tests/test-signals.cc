#include "utest/macros.h"
#include "signals.h"

#include <map>

namespace unittests {
namespace signal {

class A {
public:
	A()
	:
		foo_called_(false),
		foo_arg_(),
		bar_called_(false)
	{ }

	void foo(std::string const& arg)
	{
		foo_called_ = true;
		foo_arg_ = arg;
	}

	void bar() const
	{
		bar_called_ = true;
	}

	bool foo_called_;
	std::string foo_arg_;
	mutable bool bar_called_;
};

struct CallResult {
	bool called{false};

	std::function<void(void)> fn()
	{
		return [this]() { called = true; };
	}
};

UTEST_CASE(test_arg0)
{
	CallResult res;
	Signal<void(void)> sig;
	sig.connect(res.fn());
	sig();
	UTEST_ASSERT(res.called);
}

UTEST_CASE(test_arg1)
{
	bool called{false};
	int arg{0};
	Signal<void(int)> sig;
	sig.connect([&called, &arg](auto a) { called = true; arg = a; });
	sig(1);
	UTEST_ASSERT(called);
	UTEST_ASSERT_EQUAL(1, arg);
}

UTEST_CASE(test_arg2)
{
	bool called{false};
	int arg0{0};
	int arg1{0};
	Signal<void(int, int)> sig;
	sig.connect([&called, &arg0, &arg1](auto a0, auto a1) {
		called = true; arg0 = a0; arg1 = a1; });
	sig(1, 2);
	UTEST_ASSERT(called);
	UTEST_ASSERT_EQUAL(1, arg0);
	UTEST_ASSERT_EQUAL(2, arg1);
}

UTEST_CASE(test_memfun)
{
	A a;
	Signal<void(std::string)> sig;
	sig.connect(std::bind(&A::foo,
		&a, std::placeholders::_1));
	sig("hello");
	UTEST_ASSERT(a.foo_called_);
	UTEST_ASSERT_EQUAL("hello", a.foo_arg_);
}

UTEST_CASE(test_const_memfun)
{
	A a;
	Signal<void(void)> sig;
	sig.connect(std::bind(&A::bar, std::ref(a)));
	sig();
	UTEST_ASSERT(a.bar_called_);
}

class B {
public:
	SignalProxy<void(void)> & sig()
	{
		return sig_;
	}

	SignalProxy<void(void)> & sigr;

	B() : sigr(sig_) {}

	void run()
	{
		sig_();
	}

private:
	Signal<void(void)> sig_;
};

UTEST_CASE(test_signal_proxy)
{
	B b;

	CallResult res1;
	CallResult res2;
	b.sig().connect(res1.fn());
	b.sigr.connect(res2.fn());
	b.run();

	UTEST_ASSERT(res1.called);
	UTEST_ASSERT(res2.called);
}

void count_call(size_t & called, size_t index)
{
	UTEST_ASSERT_EQUAL(index, called);
	++called;
}

UTEST_CASE(test_call_order)
{
	Signal<void(void)> sig;
	size_t called(0);
	sig.connect(std::bind(
		&count_call, std::ref(called), 0));
	sig.connect(std::bind(
		&count_call, std::ref(called), 1));
	sig.connect(std::bind(
		&count_call, std::ref(called), 2));
	sig.connect(std::bind(
		&count_call, std::ref(called), 3));
	sig.connect(std::bind(
		&count_call, std::ref(called), 4));
	UTEST_ASSERT_EQUAL(size_t(5), sig.slots());
	sig();
	UTEST_ASSERT_EQUAL(size_t(5), called);
}

UTEST_CASE(test_connection_disconnect)
{
	CallResult res1;
	CallResult res2;
	Signal<void(void)> sig;
	auto conn1(sig.connect(res1.fn()));
	UTEST_ASSERT(conn1.connected());
	UTEST_ASSERT_EQUAL(size_t(1), sig.slots());

	auto conn2(sig.connect(res2.fn()));
	UTEST_ASSERT(conn2.connected());
	UTEST_ASSERT_EQUAL(size_t(2), sig.slots());

	sig();
	UTEST_ASSERT(res1.called);
	UTEST_ASSERT(res2.called);

	conn1.disconnect();
	UTEST_ASSERT(!conn1.connected());
	UTEST_ASSERT(conn2.connected());
	UTEST_ASSERT_EQUAL(size_t(1), sig.slots());

	res1.called = false;
	res2.called = false;
	sig();
	UTEST_ASSERT(!res1.called);
	UTEST_ASSERT(res2.called);
}

UTEST_CASE(test_connection)
{
	CallResult res;
	Signal<void(void)> sig;

	Connection conn;
	UTEST_ASSERT(!conn.connected());
	conn.disconnect();
	UTEST_ASSERT(!conn.connected());
	UTEST_ASSERT_EQUAL(size_t(0), sig.slots());

	auto conn1(sig.connect(res.fn()));
	UTEST_ASSERT_EQUAL(size_t(1), sig.slots());
	UTEST_ASSERT(conn1.connected());

	conn = conn1;
	UTEST_ASSERT(conn.connected());

	auto conn2(conn);
	UTEST_ASSERT(conn2.connected());

	UTEST_ASSERT_EQUAL(size_t(1), sig.slots());
	conn2.disconnect();
	UTEST_ASSERT_EQUAL(size_t(0), sig.slots());
	UTEST_ASSERT(!conn.connected());
	UTEST_ASSERT(!conn1.connected());
	UTEST_ASSERT(!conn2.connected());
}

void disconnect(bool & called, Connection & conn)
{
	called = true;
	conn.disconnect();
	UTEST_ASSERT(!conn.connected());
}

UTEST_CASE(test_connection_disconnect_self)
{
	bool called(false);
	Signal<void(void)> sig;
	Connection conn;
	auto conn1(sig.connect(std::bind(
		&disconnect, std::ref(called), std::ref(conn))));
	UTEST_ASSERT_EQUAL(size_t(1), sig.slots());

	conn = conn1;
	UTEST_ASSERT(conn.connected());
	UTEST_ASSERT(conn1.connected());

	sig();
	UTEST_ASSERT(called);
	UTEST_ASSERT(!conn.connected());
	UTEST_ASSERT(!conn1.connected());
	UTEST_ASSERT_EQUAL(size_t(0), sig.slots());

	called = false;
	sig();
	UTEST_ASSERT(!called);
}

UTEST_CASE(test_connection_disconnect_next)
{
	bool called(false);
	Signal<void(void)> sig;
	Connection conn;
	auto conn1(sig.connect(std::bind(
		&disconnect, std::ref(called), std::ref(conn))));

	CallResult res;
	auto conn2(sig.connect(res.fn()));
	UTEST_ASSERT_EQUAL(size_t(2), sig.slots());

	conn = conn2;
	UTEST_ASSERT(conn.connected());
	UTEST_ASSERT(conn1.connected());
	UTEST_ASSERT(conn2.connected());

	sig();
	UTEST_ASSERT_EQUAL(size_t(1), sig.slots());
	UTEST_ASSERT(called);
	UTEST_ASSERT(!res.called);
	UTEST_ASSERT(conn1.connected());
	UTEST_ASSERT(!conn.connected());
	UTEST_ASSERT(!conn2.connected());

	called = false;
	res.called = false;
	sig();
	UTEST_ASSERT(called);
	UTEST_ASSERT(!res.called);
}

UTEST_CASE(test_connection_disconnect_prev)
{
	bool called(false);
	Signal<void(void)> sig;
	Connection conn;

	CallResult res;
	auto conn1(sig.connect(res.fn()));
	auto conn2(sig.connect(std::bind(
		&disconnect, std::ref(called), std::ref(conn))));
	UTEST_ASSERT_EQUAL(size_t(2), sig.slots());

	conn = conn1;
	UTEST_ASSERT(conn.connected());
	UTEST_ASSERT(conn1.connected());
	UTEST_ASSERT(conn2.connected());

	sig();
	UTEST_ASSERT_EQUAL(size_t(1), sig.slots());
	UTEST_ASSERT(called);
	UTEST_ASSERT(res.called);
	UTEST_ASSERT(!conn1.connected());
	UTEST_ASSERT(!conn.connected());
	UTEST_ASSERT(conn2.connected());

	called = false;
	res.called = false;
	sig();
	UTEST_ASSERT(called);
	UTEST_ASSERT(!res.called);
}

class E {
public:
	E(Signal<void(void)> & sig)
	:
		called(0)
	{
		conn_.push_back(sig.connect(std::bind(&E::foo, this)));
		conn_.push_back(sig.connect(std::bind(&E::foo, this)));
		conn_.push_back(sig.connect(std::bind(&E::foo, this)));
		conn_.push_back(sig.connect(std::bind(&E::foo, this)));
		conn_.push_back(sig.connect(std::bind(&E::disconnect_all, this)));
		conn_.push_back(sig.connect(std::bind(&E::foo, this)));
		conn_.push_back(sig.connect(std::bind(&E::foo, this)));
		conn_.push_back(sig.connect(std::bind(&E::foo, this)));
	}

	void foo()
	{
		++called;
	}

	void disconnect_all()
	{
		++called;
		for (auto i(0U); i < conn_.size(); ++i) {
			UTEST_ASSERT(conn_[i].connected());
			conn_[i].disconnect();
			UTEST_ASSERT(!conn_[i].connected());
		}
	}

	std::vector<Connection> conn_;
	size_t called;
};

UTEST_CASE(test_connection_disconnect_all)
{
	Signal<void(void)> sig;
	E e(sig);

	UTEST_ASSERT_EQUAL(size_t(8), sig.slots());
	sig();
	UTEST_ASSERT_EQUAL(size_t(0), sig.slots());
	UTEST_ASSERT_EQUAL(size_t(5), e.called);

	e.called = 0;
	sig();
	UTEST_ASSERT_EQUAL(size_t(0), e.called);
}

class C {
public:
	SignalProxy<void(void)> & sig;

	C() : sig(sig_) {}

	void run() const
	{
		sig_();
	}

private:
	Signal<void(void)> sig_;
};

UTEST_CASE(test_const_signal)
{
	C c;

	CallResult res;
	c.sig.connect(res.fn());
	c.run();

	UTEST_ASSERT(res.called);
}

void connect(size_t & called, Signal<void(void)> & sig)
{
	++called;
	sig.connect(std::bind(&connect, std::ref(called), std::ref(sig)));
}

UTEST_CASE(test_connect_in_callback)
{
	size_t called(0);
	Signal<void(void)> sig;

	sig.connect(std::bind(&connect, std::ref(called), std::ref(sig)));
	UTEST_ASSERT_EQUAL(size_t(1), sig.slots());
	sig();
	UTEST_ASSERT_EQUAL(size_t(1), called);
	UTEST_ASSERT_EQUAL(size_t(2), sig.slots());
	called = 0;

	sig();
	UTEST_ASSERT_EQUAL(size_t(2), called);
	UTEST_ASSERT_EQUAL(size_t(4), sig.slots());
}

UTEST_CASE(test_auto_connection)
{
	CallResult res1;
	CallResult res2;
	CallResult res3;
	Signal<void(void)> sig;

	AutoConnection conn1(sig.connect(res1.fn()));
	UTEST_ASSERT(conn1.connected());
	{
		AutoConnection conn2(sig.connect(res2.fn()));
		UTEST_ASSERT(conn2.connected());
		UTEST_ASSERT_EQUAL(size_t(2), sig.slots());

		AutoConnection conn3(sig.connect(res3.fn()));
		UTEST_ASSERT(conn3.connected());
		UTEST_ASSERT_EQUAL(size_t(3), sig.slots());

		sig();

		UTEST_ASSERT(res1.called);
		UTEST_ASSERT(res2.called);
		UTEST_ASSERT(res3.called);
		res1.called = false;
		res2.called = false;
		res3.called = false;

		conn3.disconnect();
		UTEST_ASSERT(!conn3.connected());
		UTEST_ASSERT_EQUAL(size_t(2), sig.slots());

		sig();

		UTEST_ASSERT(res1.called);
		UTEST_ASSERT(res2.called);
		UTEST_ASSERT(!res3.called);
		res1.called = false;
		res2.called = false;
	}

	UTEST_ASSERT_EQUAL(size_t(1), sig.slots());
	UTEST_ASSERT(conn1.connected());

	sig();
	UTEST_ASSERT(res1.called);
	UTEST_ASSERT(!res2.called);
	UTEST_ASSERT(!res3.called);
}

struct functor0 {
	functor0()
	:
		called_(false)
	{ }

	void operator()()
	{
		called_ = true;
	}

	bool called_;
};

UTEST_CASE(test_functor_0arg)
{
	Signal<void(void)> sig;

	functor0 f0;

	// this will create a copy of f0!
	sig.connect(f0);
	sig();
	UTEST_ASSERT(!f0.called_);

	// this will not
	sig.connect(std::ref(f0));
	sig();

	UTEST_ASSERT(f0.called_);
}

UTEST_CASE(test_signal_deconstruct)
{
	CallResult res;
	Connection conn;
	UTEST_ASSERT(!conn.connected());
	{
		Signal<void(void)> sig;
		conn = sig.connect(res.fn());
		UTEST_ASSERT(conn.connected());
	}
	UTEST_ASSERT(!conn.connected());
}

UTEST_CASE(test_connection_assign)
{
	CallResult res1;
	Signal<void(void)> sig1;
	auto conn1(sig1.connect(res1.fn()));
	UTEST_ASSERT(conn1.connected());
	UTEST_ASSERT_EQUAL(size_t(1), sig1.slots());

	CallResult res2;
	Signal<void(void)> sig2;
	auto conn2(sig2.connect(res2.fn()));
	UTEST_ASSERT(conn2.connected());
	UTEST_ASSERT_EQUAL(size_t(1), sig2.slots());

	conn2 = conn1;
	UTEST_ASSERT_EQUAL(size_t(1), sig1.slots());
	UTEST_ASSERT_EQUAL(size_t(1), sig2.slots());

	conn2.disconnect();
	UTEST_ASSERT_EQUAL(size_t(0), sig1.slots());
	UTEST_ASSERT_EQUAL(size_t(1), sig2.slots());

	sig1();
	UTEST_ASSERT(!res1.called);

	sig2();
	UTEST_ASSERT(res2.called);
}

class producer {
public:
	SignalProxy<void(void)> & signal_my_event()
	{
		return on_event_;
	}

	// test helper
	void trigger()
	{
		on_event_();
	}

private:
	Signal<void(void)> on_event_;
};

// The purpose of this class is to keep the actual functions handling signals
// private but expose an interface to connect objects of two classes in
// a typesafe manner. The signature of the signal thus becomes the only
// compile time dependency between those two. This particular slot type
// allows to connect a slot to multiple signals as long as they share the
// same signature. The drawback is an additional copy of the callback.
template <typename T>
class slot {
public:
	slot(std::function<T> const& cb)
	:
		cb_(cb)
	{ }

	void disconnect()
	{
		std::for_each(conn_.begin(), conn_.end(),
			[](Connection & conn){ conn.disconnect(); });
		conn_.clear();
	}

	~slot()
	{
		cb_ = 0;
		disconnect();
	}

private:
	// behold the lovely syntax for defining a free standing function ...
	friend void connect(slot<T> & sl, SignalProxy<T> & sig)
	{
		UTEST_ASSERT(sl.cb_);
		sl.conn_.push_back(sig.connect(sl.cb_));
	}

	std::function<T> cb_;
	std::vector<Connection> conn_;
};

class consumer {
public:
	slot<void(void)> & slot_my_event()
	{
		return on_event_;
	}

	consumer()
	:
		count(0),
		// beware of virtual functions here
		on_event_(std::bind(&consumer::on_event, this))
	{ }

	size_t count;

private:
	void on_event()
	{
		++count;
	}

	slot<void(void)> on_event_;
};

UTEST_CASE(test_loose_coupling)
{
	producer p;
	consumer c;

	connect(c.slot_my_event(), p.signal_my_event());

	p.trigger();
	p.trigger();
	p.trigger();

	UTEST_ASSERT_EQUAL(size_t(3), c.count);
}

UTEST_CASE(test_movable_signal)
{
	CallResult res_foo;
	CallResult res_bar;
	CallResult res_baz;
	std::map<std::string, Signal<void(void)>> sig;
	sig["foo"].connect(res_foo.fn());
	sig["bar"].connect(res_bar.fn());
	sig["baz"].connect(res_baz.fn());

	sig["foo"]();
	UTEST_ASSERT(res_foo.called);
	sig["bar"]();
	UTEST_ASSERT(res_bar.called);
	sig["baz"]();
	UTEST_ASSERT(res_baz.called);

	UTEST_ASSERT_EQUAL(size_t(1), sig.erase("foo"));
	res_foo.called = false;
	sig["foo"]();
	UTEST_ASSERT(!res_foo.called);
}

UTEST_CASE(test_movable_auto_connection)
{
	Signal<void(void)> sig1;
	Signal<void(void)> sig2;
	Signal<void(void)> sig3;

	CallResult res1;
	CallResult res2;
	CallResult res3;

	{
		std::vector<AutoConnection> conn;
		conn.push_back(sig1.connect(res1.fn()));
		conn.push_back(sig2.connect(res2.fn()));
		conn.push_back(sig3.connect(res3.fn()));
		sig1();
		sig2();
		sig3();
		UTEST_ASSERT(res1.called);
		UTEST_ASSERT(res2.called);
		UTEST_ASSERT(res3.called);
	}

	res1.called = res2.called = res3.called = false;

	sig1();
	sig2();
	sig3();
	UTEST_ASSERT(!res1.called);
	UTEST_ASSERT(!res2.called);
	UTEST_ASSERT(!res3.called);
}

UTEST_CASE(test_lambda)
{
	Signal<void(void)> sig;

	bool cap(false);
	sig.connect([&cap](){ cap = true; });

	UTEST_ASSERT(!cap);
	sig();
	UTEST_ASSERT(cap);
}

class MyCallback : public CallbackBase {
public:
	void disconnect() override
	{
		disconnect_called = true;
	}

	bool disconnect_called = false;;
};

UTEST_CASE(test_custom_callback)
{
	auto cb = std::make_shared<MyCallback>();
	auto conn = Connection(cb);
	UTEST_ASSERT(conn.connected());
	UTEST_ASSERT(!cb->disconnect_called);
	conn.disconnect();
	UTEST_ASSERT(cb->disconnect_called);
	UTEST_ASSERT(!conn.connected());

	conn = Connection(cb);
	UTEST_ASSERT(conn.connected());
	cb.reset();
	UTEST_ASSERT(!conn.connected());
}

UTEST_CASE(test_empty_slot)
{
	Slot<void(int)> slot1;
	Signal<void(int)> sig1;
	connect(slot1, sig1);
	sig1(42);
}

UTEST_CASE(test_ctor_slot)
{
	int res{0};
	Slot<void(int)> slot1{[&res](auto arg){ res = arg; }};
	Signal<void(int)> sig1;
	connect(slot1, sig1);
	sig1(42);
	UTEST_ASSERT_EQUAL(42, res);
}

UTEST_CASE(test_reset_empty_slot)
{
	Slot<void(int)> slot1;
	Signal<void(int)> sig1;
	connect(slot1, sig1);

	int res{0};
	slot1.reset([&res](auto arg){ res = arg; });

	sig1(42);
	UTEST_ASSERT_EQUAL(42, res);
}

UTEST_CASE(test_reset_slot)
{
	Slot<void(int)> slot1;
	Signal<void(int)> sig1;
	connect(slot1, sig1);

	int res{0};
	slot1.reset([&res](auto arg){ res = arg; });

	sig1(42);
	UTEST_ASSERT_EQUAL(42, res);

	int res1{0};
	slot1.reset([&res1](auto arg){ res1 = arg; });
	sig1(43);
	UTEST_ASSERT_EQUAL(43, res1);
	UTEST_ASSERT_EQUAL(42, res);
}

UTEST_CASE(test_slot_multiple_signals)
{
	int res{0};
	Slot<void(int)> slot1{[&res](auto arg){ res = arg; }};

	Signal<void(int)> sig1;
	connect(slot1, sig1);

	Signal<void(int)> sig2;
	connect(slot1, sig2);

	sig1(42);
	UTEST_ASSERT_EQUAL(42, res);
	sig2(43);
	UTEST_ASSERT_EQUAL(43, res);
}

UTEST_CASE(test_slot_disconnect_all)
{
	int res{0};
	Slot<void(int)> slot1{[&res](auto arg){ res = arg; }};

	Signal<void(int)> sig1;
	connect(slot1, sig1);

	Signal<void(int)> sig2;
	connect(slot1, sig2);

	slot1.disconnect();

	sig1(42);
	UTEST_ASSERT_EQUAL(0, res);
	sig2(43);
	UTEST_ASSERT_EQUAL(0, res);
}

UTEST_CASE(test_slot_auto_disconnect)
{
	Signal<void(int)> sig1;

	int res{0};
	{
		Slot<void(int)> slot1{[&res](auto arg){ res = arg; }};
		connect(slot1, sig1);
	}

	sig1(42);
	UTEST_ASSERT_EQUAL(0, res);
}

UTEST_CASE(test_slot_direct_call)
{
	int res{0};
	Slot<void(int)> slot1{[&res](auto arg){ res = arg; }};
	slot1(42);
	UTEST_ASSERT_EQUAL(42, res);
}

UTEST_CASE(test_slot_move_ctor)
{
	int res{0};
	Slot<void(int)> slot1{[&res](auto arg){ res = arg; }};
	Slot<void(int)> slot2{std::move(slot1)};
	slot1(42);
	UTEST_ASSERT_EQUAL(0, res);
	slot2(42);
	UTEST_ASSERT_EQUAL(42, res);
}

UTEST_CASE(test_slot_move_assign)
{
	int res{0};
	int res2{0};
	Slot<void(int)> slot1{[&res](auto arg){ res = arg; }};
	Slot<void(int)> slot2{[&res2](auto arg){ res2 = arg; }};

	slot2 = std::move(slot1);

	slot1(42);
	UTEST_ASSERT_EQUAL(0, res);
	UTEST_ASSERT_EQUAL(0, res2);
	slot2(42);
	UTEST_ASSERT_EQUAL(42, res);
	UTEST_ASSERT_EQUAL(0, res2);
}

UTEST_CASE(test_signal1)
{
	Signal1<void(int)> sig1;
	int res{0};
	auto conn{sig1.connect([&res](auto arg) { res = arg; })};
	sig1(42);
	UTEST_ASSERT_EQUAL(42, res);
	UTEST_ASSERT(conn.connected());

	int res2{0};
	auto conn2{sig1.connect([&res2](auto arg) { res2 = arg; })};
	UTEST_ASSERT(!conn.connected());
	UTEST_ASSERT(conn2.connected());

	sig1(43);
	UTEST_ASSERT_EQUAL(42, res);
	UTEST_ASSERT_EQUAL(43, res2);

	conn2.disconnect();
	UTEST_ASSERT(!conn2.connected());

	sig1(44);
	UTEST_ASSERT_EQUAL(42, res);
	UTEST_ASSERT_EQUAL(43, res2);
}

}}
