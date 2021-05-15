/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include "posix/signal.h"

#include <cstring>

namespace Posix {

std::string to_string(Signal sig)
{
	return ::strsignal(static_cast<int>(sig));
}

}
