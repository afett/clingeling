/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include "utest/registry.h"

namespace UTest {

Registry & Registry::get()
{
	static Registry instance{};
	return instance;
}

void Registry::register_test(std::string const& name, SourceLocation const& loc , std::function<void(void)> const& run_test)
{
	tests_.push_back({name, loc, run_test});
}

Registry::const_iterator Registry::begin() const
{
	return tests_.begin();
}

Registry::const_iterator Registry::end() const
{
	return tests_.end();
}

}
