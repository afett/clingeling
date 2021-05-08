#include <cppunit/extensions/HelperMacros.h>

#include <cstring>

#include "io/buffer.h"

namespace unittests {
namespace io_buffer {

class test : public CppUnit::TestCase {
public:
	test();
	void setUp();
	void tearDown();

private:
	void test_empty();
	void test_reserved();
	void test_one_byte();
	void test_preallocated();
	void test_grow();
	void test_fill_drain();
	void test_reclaim();
	void test_reclaim_full();
	void test_random_access();
	void test_preallocated_zero();

	CPPUNIT_TEST_SUITE(test);
	CPPUNIT_TEST(test_empty);
	CPPUNIT_TEST(test_reserved);
	CPPUNIT_TEST(test_one_byte);
	CPPUNIT_TEST(test_preallocated);
	CPPUNIT_TEST(test_grow);
	CPPUNIT_TEST(test_fill_drain);
	CPPUNIT_TEST(test_reclaim);
	CPPUNIT_TEST(test_reclaim_full);
	CPPUNIT_TEST(test_random_access);
	CPPUNIT_TEST(test_preallocated_zero);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(test);

test::test()
{ }

void test::setUp()
{ }

void test::tearDown()
{ }

void test::test_empty()
{
	IO::Buffer buf;
	CPPUNIT_ASSERT(buf.rstart() == NULL);
	CPPUNIT_ASSERT(buf.rsize() == 0);
	CPPUNIT_ASSERT(buf.wstart() == NULL);
	CPPUNIT_ASSERT(buf.wsize() == 0);

	buf.drain(0);
	CPPUNIT_ASSERT(buf.rstart() == NULL);
	CPPUNIT_ASSERT(buf.rsize() == 0);
	CPPUNIT_ASSERT(buf.wstart() == NULL);
	CPPUNIT_ASSERT(buf.wsize() == 0);

	buf.reserve(0);
	CPPUNIT_ASSERT(buf.rstart() == NULL);
	CPPUNIT_ASSERT(buf.rsize() == 0);
	CPPUNIT_ASSERT(buf.wstart() == NULL);
	CPPUNIT_ASSERT(buf.wsize() == 0);

	buf.fill(0);
	CPPUNIT_ASSERT(buf.rstart() == NULL);
	CPPUNIT_ASSERT(buf.rsize() == 0);
	CPPUNIT_ASSERT(buf.wstart() == NULL);
	CPPUNIT_ASSERT(buf.wsize() == 0);
}

void test::test_reserved()
{
	IO::Buffer buf;
	buf.reserve(4096);
	CPPUNIT_ASSERT(buf.wstart() != NULL);
	CPPUNIT_ASSERT(buf.wsize() == 4096);
}

void test::test_one_byte()
{
	IO::Buffer buf;

	buf.reserve(1);
	CPPUNIT_ASSERT(buf.rstart() != NULL);
	CPPUNIT_ASSERT(buf.rsize() == 0);
	CPPUNIT_ASSERT(buf.wstart() != NULL);
	CPPUNIT_ASSERT(buf.wsize() == 1);

	*((char *)buf.wstart()) = 42;
	buf.fill(1);
	CPPUNIT_ASSERT(buf.rstart() != NULL);
	CPPUNIT_ASSERT(buf.rsize() == 1);
	CPPUNIT_ASSERT(buf.wstart() != NULL);
	CPPUNIT_ASSERT(buf.wsize() == 0);

	CPPUNIT_ASSERT(*((char *)buf.rstart()) == 42);
	buf.drain(1);

	CPPUNIT_ASSERT(buf.rstart() != NULL);
	CPPUNIT_ASSERT(buf.rsize() == 0);
	CPPUNIT_ASSERT(buf.wstart() != NULL);
	CPPUNIT_ASSERT(buf.wsize() == 1);
}

void test::test_preallocated()
{
	IO::Buffer buf(4096);
	CPPUNIT_ASSERT(buf.rstart() != NULL);
	CPPUNIT_ASSERT(buf.rsize() == 0);
	CPPUNIT_ASSERT(buf.wstart() != NULL);
	CPPUNIT_ASSERT(buf.wsize() == 4096);

	size_t size(buf.wsize());
	memset(buf.wstart(), 42, size);
	buf.fill(size);

	CPPUNIT_ASSERT(buf.rstart() != NULL);
	CPPUNIT_ASSERT(buf.rsize() == 4096);
	CPPUNIT_ASSERT(buf.wstart() != NULL);
	CPPUNIT_ASSERT(buf.wsize() == 0);

	char *rbuf(((char *)buf.rstart()));
	for (size_t i(0); i < 4096; ++i) {
		CPPUNIT_ASSERT(rbuf[i] == 42);
		CPPUNIT_ASSERT(buf.rsize() >= 1);
		buf.drain(1);
	}

	CPPUNIT_ASSERT(buf.rsize() == 0);
	CPPUNIT_ASSERT(buf.wsize() == 4096);
}

void test::test_grow()
{
	IO::Buffer buf;

	for (size_t i(0); i < 4096; ++i) {
		buf.reserve(i);
		CPPUNIT_ASSERT(buf.wsize() == i);
	}

	for (size_t i(4096); i != 0; --i) {
		buf.reserve(i);
		CPPUNIT_ASSERT(buf.wsize() == 4096);
	}
}

void test::test_fill_drain()
{
	IO::Buffer buf;

	buf.reserve(4096);
	CPPUNIT_ASSERT(buf.wsize() == 4096);

	buf.fill(2048);
	CPPUNIT_ASSERT(buf.wsize() == 2048);
	CPPUNIT_ASSERT(buf.rsize() == 2048);
	buf.reserve(4096);
	CPPUNIT_ASSERT(buf.wsize() == 4096);
	CPPUNIT_ASSERT(buf.rsize() == 2048);
	buf.fill(2048);
	CPPUNIT_ASSERT(buf.wsize() == 2048);
	CPPUNIT_ASSERT(buf.rsize() == 4096);
	buf.drain(2048);
	CPPUNIT_ASSERT_EQUAL(size_t(2048), buf.wsize());
	CPPUNIT_ASSERT(buf.rsize() == 2048);
	buf.drain(2048);
	CPPUNIT_ASSERT_EQUAL(size_t(4096 + 2048), buf.wsize());
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

void test::test_reclaim()
{
	IO::Buffer buf;

	buf.reserve(4096);
	CPPUNIT_ASSERT(buf.wsize() == 4096);
	memset(buf.wstart(), 1, 2048);
	buf.fill(2048);
	CPPUNIT_ASSERT(buf.wsize() == 2048);
	CPPUNIT_ASSERT(buf.rsize() == 2048);
	CPPUNIT_ASSERT(memneq(buf.rstart(), 1, buf.rsize()));
	buf.drain(1024);
	CPPUNIT_ASSERT_EQUAL(size_t(2048), buf.wsize());
	CPPUNIT_ASSERT_EQUAL(size_t(1024), buf.rsize());
	buf.reserve(4096);
	CPPUNIT_ASSERT_EQUAL(size_t(4096), buf.wsize());
	CPPUNIT_ASSERT_EQUAL(size_t(1024), buf.rsize());
	CPPUNIT_ASSERT(memneq(buf.rstart(), 1, buf.rsize()));
}

void test::test_reclaim_full()
{
	IO::Buffer buf;
	buf.reserve(4096);
	CPPUNIT_ASSERT(buf.wsize() == 4096);
	memset(buf.wstart(), 1, 4096);
	buf.fill(4096);
	CPPUNIT_ASSERT(buf.wsize() == 0);
	CPPUNIT_ASSERT(buf.rsize() == 4096);
	CPPUNIT_ASSERT(memneq(buf.rstart(), 1, buf.rsize()));
	buf.drain(2048);
	CPPUNIT_ASSERT(buf.wsize() == 0);
	CPPUNIT_ASSERT(buf.rsize() == 2048);
	buf.reserve(2048);
	CPPUNIT_ASSERT(buf.wsize() == 2048);
	CPPUNIT_ASSERT(buf.rsize() == 2048);
	CPPUNIT_ASSERT(memneq(buf.rstart(), 1, buf.rsize()));
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

void test::test_random_access()
{
	struct fake_fd fd;
	fake_fd_init(&fd);
	char out[sizeof(fd.in)];
	memset(out, 0, sizeof(out));

	size_t read(0);
	IO::Buffer buf;
	for (;;) {
		buf.reserve(1024);
		CPPUNIT_ASSERT(buf.wsize() >= 1024);
		ssize_t ret(fake_read(&fd, buf.wstart(), 1024));
		if (ret == 0) {
			break;
		}

		CPPUNIT_ASSERT(ret);
		read += ret;
	}

	CPPUNIT_ASSERT_EQUAL(sizeof(fd.in), read);
}

void test::test_preallocated_zero()
{
	auto buf = IO::Buffer(0);
}

}}
