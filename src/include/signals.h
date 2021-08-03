/*
 * Copyright (c) 2021 Andreas Fett.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

class CallbackBase {
public:
	virtual void disconnect() = 0;
	virtual ~CallbackBase() = default;
};

// handle for single slot connected to a signal
class Connection {
public:
	Connection() = default;
	~Connection()
	{
		cb_.reset();
	}

	explicit Connection(std::weak_ptr<CallbackBase> const& cb)
	:
		cb_(cb)
	{ }

	// The connections share the actual callback closure if disconnect
	// is called on one of them, all are disconnected.
	Connection(Connection const& o)
	:
		cb_(o.cb_)
	{ }

	Connection(Connection && o)
	:
		cb_(std::exchange(o.cb_, {}))
	{ }

	// Assignment to a connected connection will not disconnect it but
	// leave the callback connected to the signal. There is just no
	// external reference to it anymore.
	Connection & operator=(Connection const& o)
	{
		if (&o != this) {
			cb_ = o.cb_;
		}

		return *this;
	}

	Connection & operator=(Connection && o)
	{
		if (&o != this) {
			cb_ = std::exchange(o.cb_, {});
		}
		return *this;
	}

	bool connected() const
	{
		return !cb_.expired();
	}

	void disconnect()
	{
		auto cb(cb_.lock());
		if (cb) {
			cb->disconnect();
			cb_.reset();
		}
	}

private:
	std::weak_ptr<CallbackBase> cb_;
};

// RAII wrapper for connection
class AutoConnection {
public:
	AutoConnection() = default;
	AutoConnection(AutoConnection const&) = delete;
	AutoConnection & operator=(AutoConnection const&) = delete;

	AutoConnection(Connection const& conn)
	:
		conn_(conn)
	{ }

	AutoConnection(AutoConnection && o)
	:
		conn_(std::move(o.conn_))
	{ }

	AutoConnection & operator=(AutoConnection && o)
	{
		conn_ = std::move(o.conn_);
		return *this;
	}

	void disconnect()
	{
		conn_.disconnect();
	}

	bool connected() const
	{
		return conn_.connected();
	}

	~AutoConnection()
	{
		conn_.disconnect();
	}

private:
	Connection conn_;
};

// Interface to expose a signal for a user without allowing to invoke it
template <typename T>
class SignalProxy {
public:
	virtual Connection connect(std::function<T> const&) = 0;
	virtual ~SignalProxy() = default;
};

template <typename T>
class Signal : public SignalProxy<T> {
public:
	Signal() = default;

	Signal(Signal && o)
	:
		cb_(std::move(o.cb_))
	{ }

	Signal & operator=(Signal && o)
	{
		cb_ = std::move(o.cb_);
		return *this;
	}

	// Rationale
	// The semantics of copying and assigning a signal are somewhat undefined.
	Signal(Signal const&) = delete;
	Signal & operator=(Signal const&) = delete;

	size_t slots() const
	{
		return cb_.size();
	}

	// The callback will not be called for any already active invokation
	// of this signal at the time connect() is called.
	Connection connect(std::function<T> const& cb) final
	{
		return Connection(add_callback(cb));
	}

	template <typename... Args>
	void operator()(Args && ...args)
	{
		if (!cb_.empty()) {
			call_each({cb_.begin(), cb_.end()}, std::forward<Args>(args)...);
		}
	}

	template <typename... Args>
	void operator()(Args && ...args) const
	{
		if (!cb_.empty()) {
			call_each({cb_.begin(), cb_.end()}, std::forward<Args>(args)...);
		}
	}

private:
	struct Callback : public CallbackBase {
		Callback(Signal<T> *owner_, std::function<T> const& fn_)
		:
			owner(owner_),
			fn(fn_)
		{ }

		void disconnect() final
		{
			if (owner) {
				owner->del_callback(this);
				owner = nullptr;
				fn = nullptr;
			}
		}

		Signal<T> *owner;
		std::function<T> fn;
	};

	std::shared_ptr<Callback> add_callback(std::function<T> const& fn)
	{
		cb_.push_back(std::make_shared<Callback>(this, fn));
		return cb_.back();
	}

	void del_callback(Callback * cb)
	{
		auto it(std::find_if(cb_.begin(), cb_.end(),
			[cb](auto const& ptr) { return ptr.get() == cb; }));

		it->reset();
		cb_.erase(it);
	}

	template <typename... Args>
	static void call_each(std::vector<std::weak_ptr<Callback>> cbs, Args && ...args)
	{
		for (auto ptr: cbs) {
			auto cb(ptr.lock());
			if (cb) {
				cb->fn(std::forward<Args>(args)...);
			}
		}
	}

	std::vector<std::shared_ptr<Callback>> cb_;
};

template <typename T>
class SlotProxy {
public:
	virtual Connection connect(SignalProxy<T> &) = 0;
	virtual ~SlotProxy() = default;
};

template <typename T>
class Slot : public SlotProxy<T> {
public:
	Slot() = default;
	Slot(Slot &&) = default;
	Slot & operator=(Slot &&) = default;

	Slot(Slot const&) = delete;
	Slot & operator=(Slot const&) = delete;

	explicit Slot(std::function<T> const& fn)
	:
		fn_(fn)
	{ }

	void reset(std::function<T> const& fn)
	{
		fn_ = fn;
	}

	Connection connect(SignalProxy<T> & sig) final
	{
		auto conn{sig.connect(std::ref(*this))};
		conn_.push_back(conn);
		return conn;
	}

	void disconnect()
	{
		conn_.clear();
	}

	template <typename... Args>
	void operator()(Args && ...args)
	{
		if (fn_) {
			fn_(std::forward<Args>(args)...);
		}
	}

	template <typename... Args>
	void operator()(Args && ...args) const
	{
		if (fn_) {
			fn_(std::forward<Args>(args)...);
		}
	}

private:
	std::function<T> fn_;
	std::vector<AutoConnection> conn_;
};

template <typename T>
Connection connect(SlotProxy<T> & slot, SignalProxy<T> & sig)
{
	return slot.connect(sig);
}
