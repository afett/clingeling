/*
   Copyright (c) 2021 Andreas Fett
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include "baresip/event.h"
#include "signals.h"

namespace Baresip {

class Call {
public:
	enum class Direction {
		Incoming,
		Outgoing,
	};

	enum class State {
		Established,
		Incoming,
		Ringing,
		Closed,
	};

	class Id {
	public:
		Id() = default;
		Id(Id const&) = default;
		Id(Id &&) = default;
		Id & operator=(Id const&) = default;
		Id & operator=(Id &&) = default;

		explicit Id(std::string const& value)
		:
			value_(value)
		{ }

		explicit operator bool() const
		{
			return !value_.empty();
		}
	private:
		friend bool operator<(Id const& l, Id const& r)
		{
			return l.value_ < r.value_;
		}

		friend std::string to_string(Id const& id)
		{
			return id.value_;
		}

		std::string value_;
	};

	SignalProxy<void(State)> & on_state_change{on_state_change_};

	virtual Id id() const = 0;
	virtual State state() const = 0;
	virtual Direction direction() const = 0;
	virtual std::string accountaor() const = 0;
	virtual std::string peeruri() const = 0;

	virtual ~Call() = default;

protected:
	Signal<void(State)> on_state_change_;
};

class Model {
public:
	static std::unique_ptr<Model> create();

	enum class Registration {
		Unknown,
		Ok,
		Fail,
	};

	SignalProxy<void(std::shared_ptr<Call> const&)> & on_call{on_call_};

	SlotProxy<void(Event::Any const&)> & on_event{on_event_};

	virtual Registration registration() const = 0;
	virtual std::vector<std::shared_ptr<Call>> calls() const = 0;

	virtual ~Model() = default;

protected:
	Slot<void(Event::Any const&)> on_event_;
	Signal<void(std::shared_ptr<Call> const&)> on_call_;
};

inline bool operator>(Call::Id const& l, Call::Id const& r)
{
	return r < l;
}

inline bool operator==(Call::Id const& l, Call::Id const& r)
{
	return !(l < r) && !(l > r);
}

inline bool operator!=(Call::Id const& l, Call::Id const& r)
{
	return !(l == r);
}

}
