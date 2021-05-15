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

#include <string>

namespace Posix {

enum class Signal {
	Hup      =  1,
	Int      =  2,
	Quit     =  3,
	Ill      =  4,
	Trap     =  5,
	Abrt     =  6,
	Bus      =  7,
	Fpe      =  8,
	Kill     =  9,
	Usr1     = 10,
	Segv     = 11,
	Usr2     = 12,
	Pipe     = 13,
	Alrm     = 14,
	Term     = 15,
	Stkflt   = 16,
	Chld     = 17,
	Cont     = 18,
	Stop     = 19,
	Tstp     = 20,
	Ttin     = 21,
	Ttou     = 22,
	Urg      = 23,
	Xcpu     = 24,
	Xfsz     = 25,
	Vtalrm   = 26,
	Prof     = 27,
	Winch    = 28,
	Io       = 29,
	Pwr      = 30,
	Sys      = 31,
	Rtmin    = 34,
	Rtmin_1  = 35,
	Rtmin_2  = 36,
	Rtmin_3  = 37,
	Rtmin_4  = 38,
	Rtmin_5  = 39,
	Rtmin_6  = 40,
	Rtmin_7  = 41,
	Rtmin_8  = 42,
	Rtmin_9  = 43,
	Rtmin_10 = 44,
	Rtmin_11 = 45,
	Rtmin_12 = 46,
	Rtmin_13 = 47,
	Rtmin_14 = 48,
	Rtmin_15 = 49,
	Rtmax_14 = 50,
	Rtmax_13 = 51,
	Rtmax_12 = 52,
	Rtmax_11 = 53,
	Rtmax_10 = 54,
	Rtmax_9  = 55,
	Rtmax_8  = 56,
	Rtmax_7  = 57,
	Rtmax_6  = 58,
	Rtmax_5  = 59,
	Rtmax_4  = 60,
	Rtmax_3  = 61,
	Rtmax_2  = 62,
	Rtmax_1  = 63,
	Rtmax    = 64,
};

std::string to_string(Signal);

}

