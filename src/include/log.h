#pragma once

#include <functional>
#include "fmt.h"

namespace Log {

class Message {
public:
	explicit Message(std::string const& msg)
	:
		msg_(msg)
	{ }

	std::string get_msg() const
	{
		return msg_;
	}

private:
	std::string msg_;
};

template <typename... Args>
Message make_message(std::string const& fmt, Args ...args)
{
	return Message{Fmt::format(fmt, args...)};
}

class Logger {
public:
	void on_message(std::function<void(Message const&)> const& cb)
	{
		on_message_.push_back(cb);
	}

	void post(Message const& msg) const
	{
		for (auto const& cb : on_message_) {
			cb(msg);
		}
	}

private:
	std::vector<std::function<void(Message const&)>> on_message_;
};

}


