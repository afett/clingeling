#pragma once

#include <sstream>
#include <functional>

namespace Fmt {

template <typename... Args>
std::string format(std::string const& fmt_str, Args ...args)
{
	auto stringer = std::initializer_list<std::function<void(std::ostream &, int)>>{
		[&args]() { return [&args](std::ostream & os, int c) {
			switch (c) {
			case 'x':
				os << "0x" << std::hex << args << std::dec;
				break;
			default:
				os << args;
			}
		}; }()...
	};

	auto do_fmt{false};
	auto to_string(std::begin(stringer));
	std::stringstream ss;
	for (auto c : fmt_str) {
		if (to_string == std::end(stringer)) {
			ss << c;
			continue;
		}

		if (c == '%') {
			if (std::exchange(do_fmt, !do_fmt)) {
				ss << c;
			}
			continue;
		}

		if (std::exchange(do_fmt, false)) {
			(*to_string++)(ss, c);
			continue;
		}

		ss << c;
	}

	return ss.str();
}

}
