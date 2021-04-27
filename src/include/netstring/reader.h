#pragma once

#include <string>

namespace IO {
class StreamBuffer;
}

namespace Netstring {

class Reader {
public:
	explicit Reader(IO::StreamBuffer &);
	bool parse(std::string &);

private:
	enum class State {
		length,
		string,
		delim,
	};

	bool parse_length();
	bool parse_string() const;
	bool parse_delim();

	IO::StreamBuffer & buf_;
	State state_ = State::length;
	size_t len_ = 0;
};

}
