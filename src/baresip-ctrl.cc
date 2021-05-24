/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/
#include "baresip/ctrl.h"
#include "baresip/command.h"

#include "json/parser.h"
#include "netstring/reader.h"
#include "io/stream-buffer.h"
#include "io/event-buffer.h"
#include "baresip-proto-parser.h"

#include <sstream>

namespace Baresip {

class CtrlImpl : public Ctrl {
public:
	explicit CtrlImpl(IO::ReadEventBuffer &, IO::WriteBuffer &);
	void on_event(std::function<void(Event::Any const&)> const&) final;
	void on_response(std::function<void(Command::Response const&)> const&) final;

private:
	void on_json(Json::Object const&);

	IO::StreamBuffer recvbuf_;
	Netstring::Reader netstring_;
	IO::WriteBuffer & sendbuf_;
	std::function<void(Event::Any const&)> on_event_;
	std::function<void(Command::Response const&)> on_response_;
};

std::unique_ptr<Ctrl> Ctrl::create(IO::ReadEventBuffer & recvbuf, IO::WriteBuffer & sendbuf)
{
	return std::make_unique<CtrlImpl>(recvbuf, sendbuf);
}

CtrlImpl::CtrlImpl(IO::ReadEventBuffer & recvbuf, IO::WriteBuffer & sendbuf)
:
	recvbuf_(recvbuf),
	netstring_(recvbuf_),
	sendbuf_(sendbuf)
{
	recvbuf.on_fill([this] () {
		auto data = std::string{};
		if (netstring_.parse(data)) {
			std::stringstream ss{data};
			on_json(Json::parse_object(ss));
		}
	});
}

void CtrlImpl::on_event(std::function<void(Event::Any const&)> const& cb)
{
	on_event_ = cb;
}

void CtrlImpl::on_response(std::function<void(Command::Response const&)> const& cb)
{
	on_response_ = cb;
}

void CtrlImpl::on_json(Json::Object const& obj)
{
	auto [is_event, ev] = Event::parse(obj);
	if (is_event) {
		on_event_(ev);
	}

	auto [is_resp, resp] = Command::parse(obj);
	if (is_resp) {
		on_response_(resp);
	}
}

}
