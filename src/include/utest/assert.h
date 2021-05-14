#pragma once

#include <string>

#include "utest/assertion-traits.h"
#include "utest/source-location.h"

namespace UTest {

class AssertionFailure {
public:
	std::string msg;
	SourceLocation loc;
};

template <typename T>
void assert(T const& expr, std::string exprstr, SourceLocation loc)
{
	if (!expr) {
		throw AssertionFailure{std::move(exprstr), std::move(loc)};
	}
}

template <typename L, typename R>
void assert_equal(L const& l, R const& r, std::string expstr, std::string resstr, SourceLocation loc)
{
	if (!EqualTrait<L, R>::equal(l, r)) {
		std::string msg{"Expected: " + expstr + " '" + StringTrait<L>::to_string(l) + "' Result: " + resstr + " '" + StringTrait<R>::to_string(r) + "'"};
		throw AssertionFailure{std::move(msg), std::move(loc)};
	}
}

}
