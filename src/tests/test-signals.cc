#include "utest/macros.h"
#include "signals.h"

#include <map>

namespace unittests {
namespace signal {

struct nil {};

template <typename A0 = nil,
	  typename A1 = nil,
	  typename A2 = nil,
	  typename A3 = nil,
	  typename A4 = nil,
	  typename A5 = nil,
	  typename A6 = nil,
	  typename A7 = nil>
struct call_result {
	call_result()
	: called(false), a0(), a1(), a2(), a3(), a4(), a5(), a6(), a7()
	{ }

	bool called;
	A0 a0;
	A1 a1;
	A2 a2;
	A3 a3;
	A4 a4;
	A5 a5;
	A6 a6;
	A7 a7;
};

void f0(call_result<> & res)
{
	res.called = true;
}

template <typename A0>
void f(call_result<A0> & res, A0 a0)
{
	res.called = true;
	res.a0 = a0;
}

template <typename A0, typename A1>
void f(call_result<A0, A1> & res, A0 a0, A1 a1)
{
	res.called = true;
	res.a0 = a0;
	res.a1 = a1;
}

template <typename A0, typename A1, typename A2>
void f(call_result<A0, A1, A2> & res, A0 a0, A1 a1, A2 a2)
{
	res.called = true;
	res.a0 = a0;
	res.a1 = a1;
	res.a2 = a2;
}

template <typename A0, typename A1, typename A2, typename A3>
void f(call_result<A0, A1, A2, A3> & res, A0 a0, A1 a1, A2 a2, A3 a3)
{
	res.called = true;
	res.a0 = a0;
	res.a1 = a1;
	res.a2 = a2;
	res.a3 = a3;
}

template <typename A0, typename A1, typename A2, typename A3,
          typename A4>
void f(call_result<A0, A1, A2, A3, A4> & res, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4)
{
	res.called = true;
	res.a0 = a0;
	res.a1 = a1;
	res.a2 = a2;
	res.a3 = a3;
	res.a4 = a4;
}

template <typename A0, typename A1, typename A2, typename A3,
          typename A4, typename A5>
void f(call_result<A0, A1, A2, A3, A4, A5> & res,
       A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
{
	res.called = true;
	res.a0 = a0;
	res.a1 = a1;
	res.a2 = a2;
	res.a3 = a3;
	res.a4 = a4;
	res.a5 = a5;
}

template <typename A0, typename A1, typename A2, typename A3,
          typename A4, typename A5, typename A6>
void f(call_result<A0, A1, A2, A3, A4, A5, A6> & res,
       A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
	res.called = true;
	res.a0 = a0;
	res.a1 = a1;
	res.a2 = a2;
	res.a3 = a3;
	res.a4 = a4;
	res.a5 = a5;
	res.a6 = a6;
}

template <typename A0, typename A1, typename A2, typename A3,
          typename A4, typename A5, typename A6, typename A7>
void f(call_result<A0, A1, A2, A3, A4, A5, A6, A7> & res,
       A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
{
	res.called = true;
	res.a0 = a0;
	res.a1 = a1;
	res.a2 = a2;
	res.a3 = a3;
	res.a4 = a4;
	res.a5 = a5;
	res.a6 = a6;
	res.a7 = a7;
}

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

UTEST_CASE(test_arg0)
{
	call_result<> res;
	Signal<void(void)> sig;
	sig.connect(std::bind(
		&f0, std::ref(res)));
	sig();
	UTEST_ASSERT(res.called);
}

UTEST_CASE(test_arg1)
{
	call_result<int> res;
	Signal<void(int)> sig;
	sig.connect(std::bind(
		&f<int>, std::ref(res),
		std::placeholders::_1));
	sig(1);
	UTEST_ASSERT(res.called);
	UTEST_ASSERT(res.a0 == 1);
}

UTEST_CASE(test_arg2)
{
	call_result<int, int> res;
	Signal<void(int, int)> sig;
	sig.connect(std::bind(
		&f<int, int>, std::ref(res),
		std::placeholders::_1,
		std::placeholders::_2));
	sig(1, 2);
	UTEST_ASSERT(res.called);
	UTEST_ASSERT(res.a0 == 1);
	UTEST_ASSERT(res.a1 == 2);
}

UTEST_CASE(test_arg3)
{
	call_result<int, int, int> res;
	Signal<void(int, int, int)> sig;
	sig.connect(std::bind(
		&f<int, int, int>, std::ref(res),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3));
	sig(1, 2, 3);
	UTEST_ASSERT(res.called);
	UTEST_ASSERT(res.a0 == 1);
	UTEST_ASSERT(res.a1 == 2);
	UTEST_ASSERT(res.a2 == 3);
}

UTEST_CASE(test_arg4)
{
	call_result<int, int, int, int> res;
	Signal<void(int, int, int, int)> sig;
	sig.connect(std::bind(
		&f<int, int, int, int>, std::ref(res),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3,
		std::placeholders::_4));
	sig(1, 2, 3, 4);
	UTEST_ASSERT(res.called);
	UTEST_ASSERT(res.a0 == 1);
	UTEST_ASSERT(res.a1 == 2);
	UTEST_ASSERT(res.a2 == 3);
	UTEST_ASSERT(res.a3 == 4);
}

UTEST_CASE(test_arg5)
{
	call_result<int, int, int, int, int> res;
	Signal<void(int, int, int, int, int)> sig;
	sig.connect(std::bind(
		&f<int, int, int, int, int>, std::ref(res),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3,
		std::placeholders::_4,
		std::placeholders::_5));
	sig(1, 2, 3, 4, 5);
	UTEST_ASSERT(res.called);
	UTEST_ASSERT(res.a0 == 1);
	UTEST_ASSERT(res.a1 == 2);
	UTEST_ASSERT(res.a2 == 3);
	UTEST_ASSERT(res.a3 == 4);
	UTEST_ASSERT(res.a4 == 5);
}

UTEST_CASE(test_arg6)
{
	call_result<int, int, int, int, int, int> res;
	Signal<void(int, int, int, int, int, int)> sig;
	sig.connect(std::bind(
		&f<int, int, int, int, int, int>, std::ref(res),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3,
		std::placeholders::_4,
		std::placeholders::_5,
		std::placeholders::_6));
	sig(1, 2, 3, 4, 5, 6);
	UTEST_ASSERT(res.called);
	UTEST_ASSERT(res.a0 == 1);
	UTEST_ASSERT(res.a1 == 2);
	UTEST_ASSERT(res.a2 == 3);
	UTEST_ASSERT(res.a3 == 4);
	UTEST_ASSERT(res.a4 == 5);
	UTEST_ASSERT(res.a5 == 6);
}

UTEST_CASE(test_arg7)
{
	call_result<int, int, int, int, int, int, int> res;
	Signal<void(int, int, int, int, int, int, int)> sig;
	sig.connect(std::bind(
		&f<int, int, int, int, int, int, int>, std::ref(res),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3,
		std::placeholders::_4,
		std::placeholders::_5,
		std::placeholders::_6,
		std::placeholders::_7));
	sig(1, 2, 3, 4, 5, 6, 7);
	UTEST_ASSERT(res.called);
	UTEST_ASSERT(res.a0 == 1);
	UTEST_ASSERT(res.a1 == 2);
	UTEST_ASSERT(res.a2 == 3);
	UTEST_ASSERT(res.a3 == 4);
	UTEST_ASSERT(res.a4 == 5);
	UTEST_ASSERT(res.a5 == 6);
	UTEST_ASSERT(res.a6 == 7);
}

UTEST_CASE(test_arg8)
{
	call_result<int, int, int, int, int, int, int, int> res;
	Signal<void(int, int, int, int, int, int, int, int)> sig;
	sig.connect(std::bind(
		&f<int, int, int, int, int, int, int, int>, std::ref(res),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3,
		std::placeholders::_4,
		std::placeholders::_5,
		std::placeholders::_6,
		std::placeholders::_7,
		std::placeholders::_8));
	sig(1, 2, 3, 4, 5, 6, 7, 8);
	UTEST_ASSERT(res.called);
	UTEST_ASSERT(res.a0 == 1);
	UTEST_ASSERT(res.a1 == 2);
	UTEST_ASSERT(res.a2 == 3);
	UTEST_ASSERT(res.a3 == 4);
	UTEST_ASSERT(res.a4 == 5);
	UTEST_ASSERT(res.a5 == 6);
	UTEST_ASSERT(res.a6 == 7);
	UTEST_ASSERT(res.a7 == 8);
}

UTEST_CASE(test_memfun)
{
	A a;
	Signal<void(std::string)> sig;
	sig.connect(std::bind(&A::foo,
		&a, std::placeholders::_1));
	sig("hello");
	UTEST_ASSERT(a.foo_called_);
	UTEST_ASSERT(a.foo_arg_ == "hello");
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

	call_result<> res1;
	call_result<> res2;
	b.sig().connect(std::bind(&f0, std::ref(res1)));
	b.sigr.connect(std::bind(&f0, std::ref(res2)));
	b.run();

	UTEST_ASSERT(res1.called);
	UTEST_ASSERT(res2.called);
}

void count_call(size_t & called, size_t index)
{
	UTEST_ASSERT(called == index);
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
	UTEST_ASSERT(sig.slots() == 5);
	sig();
	UTEST_ASSERT(called == 5);
}

UTEST_CASE(test_connection_disconnect)
{
	call_result<> res1;
	call_result<> res2;
	Signal<void(void)> sig;
	auto conn1(sig.connect(std::bind(
		&f0, std::ref(res1))));
	UTEST_ASSERT(conn1.connected());
	UTEST_ASSERT(sig.slots() == 1);

	auto conn2(sig.connect(std::bind(
		&f0, std::ref(res2))));
	UTEST_ASSERT(conn2.connected());
	UTEST_ASSERT(sig.slots() == 2);

	sig();
	UTEST_ASSERT(res1.called);
	UTEST_ASSERT(res2.called);

	conn1.disconnect();
	UTEST_ASSERT(!conn1.connected());
	UTEST_ASSERT(conn2.connected());
	UTEST_ASSERT(sig.slots() == 1);

	res1.called = false;
	res2.called = false;
	sig();
	UTEST_ASSERT(!res1.called);
	UTEST_ASSERT(res2.called);
}

UTEST_CASE(test_connection)
{
	call_result<> res;
	Signal<void(void)> sig;

	Connection conn;
	UTEST_ASSERT(!conn.connected());
	conn.disconnect();
	UTEST_ASSERT(!conn.connected());
	UTEST_ASSERT(sig.slots() == 0);

	auto conn1(sig.connect(std::bind(
		&f0, std::ref(res))));
	UTEST_ASSERT(sig.slots() == 1);
	UTEST_ASSERT(conn1.connected());

	conn = conn1;
	UTEST_ASSERT(conn.connected());

	auto conn2(conn);
	UTEST_ASSERT(conn2.connected());

	UTEST_ASSERT(sig.slots() == 1);
	conn2.disconnect();
	UTEST_ASSERT(sig.slots() == 0);
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
	UTEST_ASSERT(sig.slots() == 1);

	conn = conn1;
	UTEST_ASSERT(conn.connected());
	UTEST_ASSERT(conn1.connected());

	sig();
	UTEST_ASSERT(called);
	UTEST_ASSERT(!conn.connected());
	UTEST_ASSERT(!conn1.connected());
	UTEST_ASSERT(sig.slots() == 0);

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

	call_result<> res;
	auto conn2(sig.connect(std::bind(
		&f0, std::ref(res))));
	UTEST_ASSERT(sig.slots() == 2);

	conn = conn2;
	UTEST_ASSERT(conn.connected());
	UTEST_ASSERT(conn1.connected());
	UTEST_ASSERT(conn2.connected());

	sig();
	UTEST_ASSERT(sig.slots() == 1);
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

	call_result<> res;
	auto conn1(sig.connect(std::bind(
		&f0, std::ref(res))));
	auto conn2(sig.connect(std::bind(
		&disconnect, std::ref(called), std::ref(conn))));
	UTEST_ASSERT(sig.slots() == 2);

	conn = conn1;
	UTEST_ASSERT(conn.connected());
	UTEST_ASSERT(conn1.connected());
	UTEST_ASSERT(conn2.connected());

	sig();
	UTEST_ASSERT(sig.slots() == 1);
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

	UTEST_ASSERT(sig.slots() == 8);
	sig();
	UTEST_ASSERT(sig.slots() == 0);
	UTEST_ASSERT(e.called == 5);

	e.called = 0;
	sig();
	UTEST_ASSERT(e.called == 0);
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

	call_result<> res;
	c.sig.connect(std::bind(&f0, std::ref(res)));
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
	UTEST_ASSERT(sig.slots() == 1);
	sig();
	UTEST_ASSERT(called == 1);
	UTEST_ASSERT(sig.slots() == 2);
	called = 0;

	sig();
	UTEST_ASSERT(called == 2);
	UTEST_ASSERT(sig.slots() == 4);
}

UTEST_CASE(test_auto_connection)
{
	call_result<> res1;
	call_result<> res2;
	call_result<> res3;
	Signal<void(void)> sig;

	AutoConnection conn1(sig.connect(std::bind(
		&f0, std::ref(res1))));
	UTEST_ASSERT(conn1.connected());
	{
		AutoConnection conn2(sig.connect(std::bind(
			&f0, std::ref(res2))));
		UTEST_ASSERT(conn2.connected());
		UTEST_ASSERT(sig.slots() == 2);

		AutoConnection conn3(sig.connect(std::bind(
			&f0, std::ref(res3))));
		UTEST_ASSERT(conn3.connected());
		UTEST_ASSERT(sig.slots() == 3);

		sig();

		UTEST_ASSERT(res1.called);
		UTEST_ASSERT(res2.called);
		UTEST_ASSERT(res3.called);
		res1.called = false;
		res2.called = false;
		res3.called = false;

		conn3.disconnect();
		UTEST_ASSERT(!conn3.connected());
		UTEST_ASSERT(sig.slots() == 2);

		sig();

		UTEST_ASSERT(res1.called);
		UTEST_ASSERT(res2.called);
		UTEST_ASSERT(!res3.called);
		res1.called = false;
		res2.called = false;
	}

	UTEST_ASSERT(sig.slots() == 1);
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
	call_result<> res;
	Connection conn;
	UTEST_ASSERT(!conn.connected());
	{
		Signal<void(void)> sig;
		conn = sig.connect(std::bind(&f0, std::ref(res)));
		UTEST_ASSERT(conn.connected());
	}
	UTEST_ASSERT(!conn.connected());
}

UTEST_CASE(test_connection_assign)
{
	call_result<> res1;
	Signal<void(void)> sig1;
	auto conn1(sig1.connect(std::bind(&f0, std::ref(res1))));
	UTEST_ASSERT(conn1.connected());
	UTEST_ASSERT(sig1.slots() == 1);

	call_result<> res2;
	Signal<void(void)> sig2;
	auto conn2(sig2.connect(std::bind(&f0, std::ref(res2))));
	UTEST_ASSERT(conn2.connected());
	UTEST_ASSERT(sig2.slots() == 1);

	conn2 = conn1;
	UTEST_ASSERT(sig1.slots() == 1);
	UTEST_ASSERT(sig2.slots() == 1);

	conn2.disconnect();
	UTEST_ASSERT(sig1.slots() == 0);
	UTEST_ASSERT(sig2.slots() == 1);

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

	UTEST_ASSERT(c.count == 3);
}

UTEST_CASE(test_movable_signal)
{
	call_result<> res_foo;
	call_result<> res_bar;
	call_result<> res_baz;
	std::map<std::string, Signal<void(void)>> sig;
	sig["foo"].connect(std::bind(&f0, std::ref(res_foo)));
	sig["bar"].connect(std::bind(&f0, std::ref(res_bar)));
	sig["baz"].connect(std::bind(&f0, std::ref(res_baz)));

	sig["foo"]();
	UTEST_ASSERT(res_foo.called);
	sig["bar"]();
	UTEST_ASSERT(res_bar.called);
	sig["baz"]();
	UTEST_ASSERT(res_baz.called);

	UTEST_ASSERT(sig.erase("foo") == 1);
	res_foo.called = false;
	sig["foo"]();
	UTEST_ASSERT(!res_foo.called);
}

UTEST_CASE(test_movable_auto_connection)
{
	Signal<void(void)> sig1;
	Signal<void(void)> sig2;
	Signal<void(void)> sig3;

	call_result<> res1;
	call_result<> res2;
	call_result<> res3;

	{
		std::vector<AutoConnection> conn;
		conn.push_back(sig1.connect(std::bind(&f0, std::ref(res1))));
		conn.push_back(sig2.connect(std::bind(&f0, std::ref(res2))));
		conn.push_back(sig3.connect(std::bind(&f0, std::ref(res3))));
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

}}
