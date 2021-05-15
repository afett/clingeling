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

#include <cstdint>
#include <memory>

namespace USB {
namespace Hid {
namespace Usage {

enum class Page : uint16_t {
	None      = 0x0000,
	Telephony = 0x000B,
	Consumer  = 0x000C,
	LED       = 0x0008,
	Button    = 0x0009,
};

enum class Tel : uint16_t {
	Hook_Switch     = 0x0020,
	Flash           = 0x0021,
	Feature         = 0x0022,
	Hold            = 0x0023,
	Redial          = 0x0024,
	Transfer        = 0x0025,
	Drop            = 0x0026,
	Park            = 0x0027,
	Forward         = 0x0028,
	Alternate       = 0x0029,
	Line            = 0x002A,
	Speaker         = 0x002B,
	Conference      = 0x002C,
	Ring_Enable     = 0x002D,
	Ring_Select     = 0x002E,
	Phone_Mute      = 0x002F,
	Caller          = 0x0030,
	Send            = 0x0031,
	VoiceMail       = 0x0070,
	Ringer          = 0x009E,
	Phone_Key_0     = 0x00B0,
	Phone_Key_1     = 0x00B1,
	Phone_Key_2     = 0x00B2,
	Phone_Key_3     = 0x00B3,
	Phone_Key_4     = 0x00B4,
	Phone_Key_5     = 0x00B5,
	Phone_Key_6     = 0x00B6,
	Phone_Key_7     = 0x00B7,
	Phone_Key_8     = 0x00B8,
	Phone_Key_9     = 0x00B9,
	Phone_Key_Star  = 0x00BA,
	Phone_Key_Pound = 0x00BB,
	Phone_Key_A     = 0x00BC,
	Phone_Key_B     = 0x00BD,
	Phone_Key_C     = 0x00BE,
	Phone_Key_D     = 0x00BF,
	Control         = 0xFFFF,
};

enum class Consumer : uint16_t {
	Volume_Incr = 0x00E9,
	Volume_Decr = 0x00EA,
};

enum class Led : uint16_t {
	Mute       = 0x0009,
	Off_Hook   = 0x0017,
	Ring       = 0x0018,
	Hold       = 0x0020,
	Microphone = 0x0021,
	On_Line    = 0x002A,
	Off_Line   = 0x002B,
};

class Report {
public:
	explicit Report(Tel)
	{
	}

	explicit Report(Consumer)
	{
	}

	explicit Report(Led)
	{
	}

private:

};

}

class Event {
public:
	Event() = default;
	Event(Event const&) = default;
	Event & operator=(Event const&) = default;

	explicit operator bool() const
	{
		return !((hid_ == 0) && (value_ == 0));
	}

	Event(uint32_t hid, int32_t value)
	:
		hid_(hid),
		value_(value)
	{ }

	Usage::Page page() const
	{
		return Usage::Page(hid_ >> 16);
	}

	template <typename T>
	T code() const
	{
		return T(hid_ & 0xFFFF);
	}

private:
	uint32_t hid_ = 0;
	int32_t value_ = 0;
};

}}

namespace Posix {
class CharDev;
}

namespace USB {
namespace Hid {

class Device {
public:
	static std::shared_ptr<Device> create(std::shared_ptr<Posix::CharDev> const&);
	virtual Event read_event() = 0;
	virtual void write(Usage::Report) = 0;
	virtual int32_t read(Usage::Report) = 0;
	virtual ~Device() = default;
};

}}
