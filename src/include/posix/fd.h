#pragma once

#include <memory>

namespace Posix {

class Fd {
public:
	explicit Fd(int fd)
	:
		fd_(new Priv{fd})
	{ }

	Fd() = default;
	Fd(Fd const&) = default;
	Fd(Fd &&) = default;
	Fd & operator=(Fd const&) = default;
	Fd & operator=(Fd &&) = default;

	inline explicit operator bool() const
	{
		return fd_ != nullptr;
	}

	inline int get() const
	{
		return fd_->fd;
	}

private:
	struct Priv {
		int fd = -1;

		~Priv();
	};

	std::shared_ptr<Priv> fd_ = {nullptr};
};

}
