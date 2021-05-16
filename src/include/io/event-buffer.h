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

#include <io/buffer.h>

#include <functional>

namespace IO {

class ReadEventBuffer : public IO::ReadBuffer {
public:
	virtual void on_fill(std::function<void(void)> const&) = 0;
};

class WriteEventBuffer : public IO::WriteBuffer {
public:
	virtual void on_drain(std::function<void(void)> const&) = 0;
};

class EventBuffer : public ReadEventBuffer, public WriteEventBuffer {
public:
	EventBuffer() = default;

	explicit EventBuffer(size_t size)
	:
		buf_(size)
	{ }

	void *rstart() const final
	{
		return buf_.rstart();
	}

	size_t rsize() const final
	{
		return buf_.rsize();
	}

	bool empty() const final
	{
		return buf_.empty();
	}

	void drain(size_t size) final
	{
		buf_.drain(size);
		on_drain_();
	}

	void *wstart() const final
	{
		return buf_.wstart();
	}

	size_t wsize() const final
	{
		return buf_.wsize();
	}

	void reserve(size_t size) final
	{
		return buf_.reserve(size);
	}

	bool full() const final
	{
		return buf_.full();
	}

	void fill(size_t size) final
	{
		buf_.fill(size);
		on_fill_();
	}

	void on_fill(std::function<void(void)> const& cb) final
	{
		on_fill_ = cb;
	}

	void on_drain(std::function<void(void)> const& cb) final
	{
		on_drain_ = cb;
	}

private:
	IO::Buffer buf_;
	std::function<void(void)> on_drain_;
	std::function<void(void)> on_fill_;
};

}
