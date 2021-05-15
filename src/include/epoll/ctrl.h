/*
   Copyright (c) 2021 Andreas Fett
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include "flags.h"

#include <chrono>
#include <functional>
#include <memory>

namespace Posix {
class Fd;
}

namespace EPoll {

enum class Event {
	In = 0,
	Out,
	RdHup,
	Pri,
	Err,
	Hup,
};

using Events = Flags<Event>;

class Ctrl {
public:
	virtual void add(std::shared_ptr<Posix::Fd> const&, Events const&, std::function<void(Events const&)> const&) = 0;
	virtual void del(std::shared_ptr<Posix::Fd> const&) = 0;
	virtual void mod(std::shared_ptr<Posix::Fd> const&, Events const&) const = 0;

	static std::chrono::milliseconds Infinity();
	virtual bool wait(std::chrono::milliseconds const& = Infinity()) const = 0;

	virtual ~Ctrl() = default;
};

class CtrlFactory {
public:
	static std::unique_ptr<CtrlFactory> create();
	virtual std::unique_ptr<Ctrl> make_ctrl() const = 0;
	virtual ~CtrlFactory() = default;
};

}

DECLARE_FLAG_TYPE(EPoll::Event);
