#pragma once

#include <memory>

namespace Posix {

class Fd;

class PipeFactory {
public:
	struct Params {
		bool nonblock = false;
		bool cloexec = false;
	};

	static std::unique_ptr<PipeFactory> create();
	virtual std::pair<Fd, Fd> make_pipe(Params const&) const = 0;
	virtual ~PipeFactory() = default;
};

}
