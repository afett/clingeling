#include "utest/macros.h"

#include <cstring>

#include "io/buffer.h"

namespace unittests {
namespace io_buffer {

UTEST_CASE(test_empty)
{
	IO::Buffer buf;
	UTEST_ASSERT(buf.rstart() == NULL);
	UTEST_ASSERT(buf.rsize() == 0);
	UTEST_ASSERT(buf.wstart() == NULL);
	UTEST_ASSERT(buf.wsize() == 0);

	buf.drain(0);
	UTEST_ASSERT(buf.rstart() == NULL);
	UTEST_ASSERT(buf.rsize() == 0);
	UTEST_ASSERT(buf.wstart() == NULL);
	UTEST_ASSERT(buf.wsize() == 0);

	buf.reserve(0);
	UTEST_ASSERT(buf.rstart() == NULL);
	UTEST_ASSERT(buf.rsize() == 0);
	UTEST_ASSERT(buf.wstart() == NULL);
	UTEST_ASSERT(buf.wsize() == 0);

	buf.fill(0);
	UTEST_ASSERT(buf.rstart() == NULL);
	UTEST_ASSERT(buf.rsize() == 0);
	UTEST_ASSERT(buf.wstart() == NULL);
	UTEST_ASSERT(buf.wsize() == 0);
}

UTEST_CASE(test_reserved)
{
	IO::Buffer buf;
	buf.reserve(4096);
	UTEST_ASSERT(buf.wstart() != NULL);
	UTEST_ASSERT(buf.wsize() == 4096);
}

UTEST_CASE(test_one_byte)
{
	IO::Buffer buf;

	buf.reserve(1);
	UTEST_ASSERT(buf.rstart() != NULL);
	UTEST_ASSERT(buf.rsize() == 0);
	UTEST_ASSERT(buf.wstart() != NULL);
	UTEST_ASSERT(buf.wsize() == 1);

	*((char *)buf.wstart()) = 42;
	buf.fill(1);
	UTEST_ASSERT(buf.rstart() != NULL);
	UTEST_ASSERT(buf.rsize() == 1);
	UTEST_ASSERT(buf.wstart() != NULL);
	UTEST_ASSERT(buf.wsize() == 0);

	UTEST_ASSERT(*((char *)buf.rstart()) == 42);
	buf.drain(1);

	UTEST_ASSERT(buf.rstart() != NULL);
	UTEST_ASSERT(buf.rsize() == 0);
	UTEST_ASSERT(buf.wstart() != NULL);
	UTEST_ASSERT(buf.wsize() == 1);
}

UTEST_CASE(test_preallocated)
{
	IO::Buffer buf(4096);
	UTEST_ASSERT(buf.rstart() != NULL);
	UTEST_ASSERT(buf.rsize() == 0);
	UTEST_ASSERT(buf.wstart() != NULL);
	UTEST_ASSERT(buf.wsize() == 4096);

	size_t size(buf.wsize());
	memset(buf.wstart(), 42, size);
	buf.fill(size);

	UTEST_ASSERT(buf.rstart() != NULL);
	UTEST_ASSERT(buf.rsize() == 4096);
	UTEST_ASSERT(buf.wstart() != NULL);
	UTEST_ASSERT(buf.wsize() == 0);

	char *rbuf(((char *)buf.rstart()));
	for (size_t i(0); i < 4096; ++i) {
		UTEST_ASSERT(rbuf[i] == 42);
		UTEST_ASSERT(buf.rsize() >= 1);
		buf.drain(1);
	}

	UTEST_ASSERT(buf.rsize() == 0);
	UTEST_ASSERT(buf.wsize() == 4096);
}

UTEST_CASE(test_grow)
{
	IO::Buffer buf;

	for (size_t i(0); i < 4096; ++i) {
		buf.reserve(i);
		UTEST_ASSERT(buf.wsize() == i);
	}

	for (size_t i(4096); i != 0; --i) {
		buf.reserve(i);
		UTEST_ASSERT(buf.wsize() == 4096);
	}
}

UTEST_CASE(test_fill_drain)
{
	IO::Buffer buf;

	buf.reserve(4096);
	UTEST_ASSERT(buf.wsize() == 4096);

	buf.fill(2048);
	UTEST_ASSERT(buf.wsize() == 2048);
	UTEST_ASSERT(buf.rsize() == 2048);
	buf.reserve(4096);
	UTEST_ASSERT(buf.wsize() == 4096);
	UTEST_ASSERT(buf.rsize() == 2048);
	buf.fill(2048);
	UTEST_ASSERT(buf.wsize() == 2048);
	UTEST_ASSERT(buf.rsize() == 4096);
	buf.drain(2048);
	UTEST_ASSERT_EQUAL(size_t(2048), buf.wsize());
	UTEST_ASSERT(buf.rsize() == 2048);
	buf.drain(2048);
	UTEST_ASSERT_EQUAL(size_t(4096 + 2048), buf.wsize());
}

namespace {

bool memneq(void *start, int value, size_t len)
{
	for (size_t i(0); i < len; ++i) {
		if (((char*)start)[i] != value) {
			return false;
		}
	}

	return true;
}

}

UTEST_CASE(test_reclaim)
{
	IO::Buffer buf;

	buf.reserve(4096);
	UTEST_ASSERT(buf.wsize() == 4096);
	memset(buf.wstart(), 1, 2048);
	buf.fill(2048);
	UTEST_ASSERT(buf.wsize() == 2048);
	UTEST_ASSERT(buf.rsize() == 2048);
	UTEST_ASSERT(memneq(buf.rstart(), 1, buf.rsize()));
	buf.drain(1024);
	UTEST_ASSERT_EQUAL(size_t(2048), buf.wsize());
	UTEST_ASSERT_EQUAL(size_t(1024), buf.rsize());
	buf.reserve(4096);
	UTEST_ASSERT_EQUAL(size_t(4096), buf.wsize());
	UTEST_ASSERT_EQUAL(size_t(1024), buf.rsize());
	UTEST_ASSERT(memneq(buf.rstart(), 1, buf.rsize()));
}

UTEST_CASE(test_reclaim_full)
{
	IO::Buffer buf;
	buf.reserve(4096);
	UTEST_ASSERT(buf.wsize() == 4096);
	memset(buf.wstart(), 1, 4096);
	buf.fill(4096);
	UTEST_ASSERT(buf.wsize() == 0);
	UTEST_ASSERT(buf.rsize() == 4096);
	UTEST_ASSERT(memneq(buf.rstart(), 1, buf.rsize()));
	buf.drain(2048);
	UTEST_ASSERT(buf.wsize() == 0);
	UTEST_ASSERT(buf.rsize() == 2048);
	buf.reserve(2048);
	UTEST_ASSERT(buf.wsize() == 2048);
	UTEST_ASSERT(buf.rsize() == 2048);
	UTEST_ASSERT(memneq(buf.rstart(), 1, buf.rsize()));
}

namespace {

struct fake_fd {
	size_t read;
	char in[65536];
};

void fake_fd_init(struct fake_fd *src)
{
	srandom(time(NULL));

	for (size_t i(0); i < sizeof(src->in); ++i) {
		src->in[i] = (rand() % 255) + 1;
	}

	for (size_t i(0); i < 1000; ++i) {
		size_t pos(0);
		do {
			pos = rand() % sizeof(src->in);
		} while (src->in[pos] == 0);
		src->in[pos] = 0;
	}

	src->read = 0;
}

ssize_t fake_read(struct fake_fd *src, void *buf, size_t count)
{
	if (src->read == sizeof(src->in)) {
		return 0;
	}

	size_t ret((rand() % count) + 1);
	if (src->read + ret > sizeof(src->in)) {
		ret = sizeof(src->in) - src->read;
	}

	memcpy(buf, &src->in[src->read], ret);
	src->read += ret;
	return ret;
}

}

UTEST_CASE(test_random_access)
{
	struct fake_fd fd;
	fake_fd_init(&fd);
	char out[sizeof(fd.in)];
	memset(out, 0, sizeof(out));

	size_t read(0);
	IO::Buffer buf;
	for (;;) {
		buf.reserve(1024);
		UTEST_ASSERT(buf.wsize() >= 1024);
		ssize_t ret(fake_read(&fd, buf.wstart(), 1024));
		if (ret == 0) {
			break;
		}

		UTEST_ASSERT(ret);
		read += ret;
	}

	UTEST_ASSERT_EQUAL(sizeof(fd.in), read);
}

UTEST_CASE(test_preallocated_zero)
{
	auto buf = IO::Buffer(0);
}

}}
