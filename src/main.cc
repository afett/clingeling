/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include "clingeling.h"

#include <iostream>

namespace {

void backtrace(std::exception const& e)
{
	std::cerr << e.what() << "\n";
	try {
		std::rethrow_if_nested(e);
	} catch (std::exception const& e) {
		backtrace(e);
	}
}

}

int main()
{
	try {
		return clingeling(0, nullptr);
	} catch (std::exception const& e) {
		::backtrace(e);
		return 1;
	}

	return 0;
}
