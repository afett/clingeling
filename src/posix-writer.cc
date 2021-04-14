#include <cassert>

#include <unistd.h>

#include "posix/system-error.h"
#include "posix/writer.h"
#include "posix/fd.h"

namespace Posix {

class WriterImpl : public Writer {
public:
	explicit WriterImpl(Fd const&);
	size_t write(void const*, size_t) const override;
	Fd getFd() const override;

private:
	Fd fd_;
};

WriterImpl::WriterImpl(Fd const& fd)
:
	fd_(fd)
{
	assert(fd);
	assert(fd.get() != -1);
}

size_t WriterImpl::write(void const* buf, size_t count) const
{
	int res{-1};
	do {
		res = ::write(fd_.get(), buf, count);
	} while (res == -1 && errno == EINTR);

	if (res == -1) {
		throw POSIX_SYSTEM_ERROR("::read(%s, %s, %s);", fd_.get(), buf, count);
	}

	assert(res >= 0);
	return res;
}

Fd WriterImpl::getFd() const
{
	return fd_;
}

class WriterFactoryImpl : public WriterFactory {
public:
	std::unique_ptr<Writer> make_writer(Fd const&) const override;
};

std::unique_ptr<WriterFactory> WriterFactory::create()
{
	return std::make_unique<WriterFactoryImpl>();
}

std::unique_ptr<Writer> WriterFactoryImpl::make_writer(Fd const& fd) const
{
	return std::make_unique<WriterImpl>(fd);
}

}
