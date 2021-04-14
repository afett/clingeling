#include <cassert>

#include <unistd.h>

#include "posix/system-error.h"
#include "posix/reader.h"
#include "posix/fd.h"

namespace Posix {

class ReaderImpl : public Reader {
public:
	explicit ReaderImpl(Fd const&);
	size_t read(void *, size_t) const override;
	Fd getFd() const override;

private:
	Fd fd_;
};

ReaderImpl::ReaderImpl(Fd const& fd)
:
	fd_(fd)
{
	assert(fd);
	assert(fd.get() != -1);
}

size_t ReaderImpl::read(void *buf, size_t count) const
{
	int res{-1};
	do {
		res = ::read(fd_.get(), buf, count);
	} while (res == -1 && errno == EINTR);

	if (res == -1) {
		throw POSIX_SYSTEM_ERROR("::read(%s, %s, %s);", fd_.get(), buf, count);
	}

	assert(res >= 0);
	return res;
}

Fd ReaderImpl::getFd() const
{
	return fd_;
}

class ReaderFactoryImpl : public ReaderFactory {
public:
	std::unique_ptr<Reader> make_reader(Fd const&) const override;
};

std::unique_ptr<ReaderFactory> ReaderFactory::create()
{
	return std::make_unique<ReaderFactoryImpl>();
}

std::unique_ptr<Reader> ReaderFactoryImpl::make_reader(Fd const& fd) const
{
	return std::make_unique<ReaderImpl>(fd);
}

}
