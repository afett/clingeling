/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/
#include "baresip/model.h"

namespace Baresip {

namespace {

Call::State call_state(Event::Call::Type type)
{
	switch (type) {
	case Event::Call::Type::Established: return Call::State::Established;
	case Event::Call::Type::Incoming: return Call::State::Incoming;
	case Event::Call::Type::Ringing: return Call::State::Ringing;
	case Event::Call::Type::Closed: return Call::State::Closed;
	}

	return Call::State::Closed;
}

Call::Direction call_direction(Event::Call::Direction direction)
{
	switch (direction) {
	case Event::Call::Direction::Incoming: return Call::Direction::Incoming;
	case Event::Call::Direction::Outgoing: return Call::Direction::Outgoing;
	}

	return Call::Direction::Outgoing;
}

}

class CallImpl : public Call {
public:
	explicit CallImpl(Event::Call const& ev)
	:
		id_(ev.id),
		state_(call_state(ev.type)),
		direction_(call_direction(ev.direction)),
		accountaor_(ev.accountaor),
		peeruri_(ev.peeruri)
	{ }

	bool on_event(Event::Call const& ev)
	{
		if (Id{ev.id} != id_) {
			return false;
		}

		auto state = call_state(ev.type);
		if (state_ != state) {
			state_ = state;
			on_state_change_(state_);
		}
		return true;
	}

	Id id() const final;
	State state() const final;
	Direction direction() const final;
	std::string accountaor() const final;
	std::string peeruri() const final;

private:
	Id id_;
	State state_;
	Direction direction_;
	std::string accountaor_;
	std::string peeruri_;
};

class ModelImpl : public Model {
public:
	ModelImpl()
	:
		event_handler_(*this)
	{ }

	void on_event(Event::Any const&) final;
	Registration registration() const final;
	std::vector<std::shared_ptr<Call>> calls() const final;

private:
	void on_call_event(Event::Call const&);
	void on_register_event(Event::Register const&);

	friend class EventHandler;

	class EventHandler {
	public:
		explicit EventHandler(ModelImpl & model)
		:
			model_(model)
		{ }

		void operator()(Event::Call const& ev)
		{
			model_.on_call_event(ev);
		}

		void operator()(Event::Register const& ev)
		{
			model_.on_register_event(ev);
		}

	private:
		ModelImpl & model_;
	};

	EventHandler event_handler_;
	std::vector<std::shared_ptr<CallImpl>> calls_;
	Registration registration_ = Registration::Unknown;
};

std::unique_ptr<Model> Model::create()
{
	return std::make_unique<ModelImpl>();
}

Call::Id CallImpl::id() const
{
	return id_;
}

Call::State CallImpl::state() const
{
	return state_;
}

Call::Direction CallImpl::direction() const
{
	return direction_;
}

std::string CallImpl::accountaor() const
{
	return accountaor_;
}

std::string CallImpl::peeruri() const
{
	return peeruri_;
}

void ModelImpl::on_event(Event::Any const& ev)
{
	std::visit(event_handler_, ev);
}

void ModelImpl::on_call_event(Event::Call const& ev)
{
	auto new_call = std::none_of(begin(calls_), end(calls_),
		[&ev](auto const& call) { return call->on_event(ev); });

	if (new_call) {
		calls_.push_back(std::make_shared<CallImpl>(ev));
		on_call_(calls_.back());
	}
}

void ModelImpl::on_register_event(Event::Register const& ev)
{
	switch (ev.type) {
	case Event::Register::Type::Fail:
		registration_ = Registration::Fail;
		break;
	case Event::Register::Type::Ok:
		registration_ = Registration::Ok;
		break;
	case Event::Register::Type::Unregistering:
		registration_ = Registration::Unknown;
		break;
	}
}

Model::Registration ModelImpl::registration() const
{
	return registration_;
}

std::vector<std::shared_ptr<Call>> ModelImpl::calls() const
{
	std::vector<std::shared_ptr<Call>> res(calls_.size());
	std::copy(begin(calls_), end(calls_), begin(res));
	return res;
}

}
