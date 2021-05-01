#include "netstring/reader.h"
#include "io/stream-buffer.h"
#include "io/buffer.h"
#include "fmt.h"

namespace Netstring {

Reader::Reader(IO::StreamBuffer & buf)
:
	buf_(buf)
{ }

bool Reader::parse(std::string & str)
{
	for (;;) {
		switch (state_) {
		case State::start:
		case State::length:
			if (parse_length()) {
				state_ = State::string;
				continue;
			}
			break;
		case State::string:
			if (parse_string()) {
				state_ = State::delim;
				continue;
			}
			break;
		case State::delim:
			if (parse_delim()) {
				str = buf_.get_str(len_);
				buf_.get(); // discard ','
				len_ = 0;
				state_ = State::length;
				return true;
			}
			break;
		}

		return false;
	}
}

bool Reader::parse_length()
{
	for (;;) {
		auto c = buf_.get();
		switch (c) {
		case '0' ... '9':
			state_ = State::length;
			len_ = len_ * 10 + c - '0';
			continue;
		case ':':
			if (state_ == State::length) {
				return true;
			}
			break;
		case IO::StreamBuffer::End:
			return false;
		}
		throw std::runtime_error(Fmt::format("unexpected character '%s' while parsing length", char(c)));
	}
}

bool Reader::parse_string() const
{
	return buf_.size() >= len_ + 1;
}

bool Reader::parse_delim()
{
	auto c = buf_.peek(len_);
	switch (c) {
	case ',':
		// TODO
		return true;
	case IO::StreamBuffer::End:
		throw std::runtime_error("unexpected buffer end");
	default:
		throw std::runtime_error(Fmt::format("unexpected character '%s' while parsing delimiter", char(c)));
	}
	return false;
}

}
