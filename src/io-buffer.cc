/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "io/buffer.h"

namespace IO {

void Buffer::fill(size_t size)
{
	assert(wstart_ + size <= capacity_);
	wstart_ += size;
}

size_t Buffer::rsize() const
{
	return wstart_ - rstart_;
}

void *Buffer::rstart() const
{
	return static_cast<char *>(data_) + rstart_;
}

size_t Buffer::wsize() const
{
	return capacity_ - wstart_;
}

void *Buffer::wstart() const
{
	return static_cast<char *>(data_) + wstart_;
}

void Buffer::drain(size_t size)
{
	assert(size <= rsize());

	rstart_ += size;
	if (rstart_ == wstart_) {
		wstart_ = 0;
		rstart_ = 0;
	}
}

bool Buffer::full() const
{
	return wsize() == 0;
}

bool Buffer::empty() const
{
	return rsize() == 0;
}

Buffer::Buffer(size_t size)
{
	grow(size);
}

Buffer::~Buffer()
{
	free(data_);
}

void Buffer::reserve(size_t size)
{
	if (size == 0) {
		return;
	}

	if (size <= wsize()) {
		return;
	}

	if (rstart_ != 0) {
		reclaim();
	}

	if (size <= wsize()) {
		return;
	}

	grow(capacity_ + (size - wsize()));
}

void Buffer::grow(size_t capacity)
{
	assert(capacity >= capacity_);

	void *n(realloc(data_, capacity));
	if (n == nullptr) {
		throw std::bad_alloc();
	}

	data_ = n;
	capacity_ = capacity;
}

void Buffer::reclaim()
{
	size_t srsize(rsize());
	memmove(data_, rstart(), srsize);
	rstart_ = 0;
	wstart_ = srsize;
}

}
