/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/
#pragma once

#include "baresip/event.h"
#include "json/types.h"

#include <tuple>

namespace Baresip {

std::tuple<bool, Event::Any> parse_event(Json::Object const&);

}
