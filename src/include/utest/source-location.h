#pragma once

#include <string>
#include <ostream>

namespace UTest {

class SourceLocation {
public:
	std::string file;
	int line;
};

inline std::ostream & operator<<(std::ostream & os, SourceLocation const& loc)
{
	return os << loc.file << ':' << loc.line << ':';
}

}
