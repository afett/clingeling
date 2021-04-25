#include <cassert>
#include <unistd.h>

#include "posix/fd.h"
#include "posix/system-error.h"

namespace Posix {

class FdImpl : public Fd {
public:
	explicit FdImpl(int fd)
	:
		fd_(fd)
	{ }

	~FdImpl()
	{
		if (fd_ != -1) {
			::close(fd_);
		}
	}

	int get() const override
	{
		return fd_;
	}

	size_t write(void const* buf, size_t count) const override
	{
		int res{-1};
		do {
			res = ::write(fd_, buf, count);
		} while (res == -1 && errno == EINTR);

		if (res == -1) {
			throw POSIX_SYSTEM_ERROR("::read(%s, %s, %s);", fd_, buf, count);
		}

		assert(res >= 0);
		return res;
	}

	size_t read(void *buf, size_t count) const override
	{
		int res{-1};
		do {
			res = ::read(fd_, buf, count);
		} while (res == -1 && errno == EINTR);

		if (res == -1) {
			throw POSIX_SYSTEM_ERROR("::read(%s, %s, %s);", fd_, buf, count);
		}

		assert(res >= 0);
		return res;
	}

private:
	int fd_ = -1;
};

std::shared_ptr<Fd> Fd::create(int fd)
{
	return std::make_shared<FdImpl>(fd);
}

}
