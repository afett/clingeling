#pragma once

#include <memory>

namespace Posix {

class Fd;

using Pipe = std::tuple<std::shared_ptr<Fd>, std::shared_ptr<Fd>>;

class PipeFactory {
public:
	struct Params {
		bool nonblock = false;
		bool cloexec = false;
	};

	static std::unique_ptr<PipeFactory> create();
	virtual Pipe make_pipe(Params const&) const = 0;
	virtual ~PipeFactory() = default;
};

}
