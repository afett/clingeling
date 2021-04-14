#include <unistd.h>
#include <fcntl.h>

#include "posix/pipe-factory.h"
#include "posix/fd.h"
#include "posix/system-error.h"

namespace Posix {

class PipeFactoryImpl : public PipeFactory {
public:
	static std::unique_ptr<PipeFactory> create();
	std::pair<Fd, Fd> make_pipe(Params const&) const override;
};

std::unique_ptr<PipeFactory> PipeFactory::create()
{
	return std::make_unique<PipeFactoryImpl>();
}

namespace {

int pipe_params(PipeFactory::Params const& params)
{
	int res{0};
	if (params.nonblock) {
		res |= O_NONBLOCK;
	}

	if (params.cloexec) {
		res |= O_CLOEXEC;
	}

	return res;
}

}

std::pair<Fd, Fd> PipeFactoryImpl::make_pipe(Params const& params) const
{
	auto pipefd = std::array<int, 2>{-1, -1};
	auto res = ::pipe2(pipefd.data(), pipe_params(params));
	if (res == -1) {
		throw make_system_error(errno, Fmt::format("::pipe2(%x, %s);",
				pipefd.data(), pipe_params(params)));
	}
	return {Fd{pipefd[0]}, Fd{pipefd[1]}};
}

}
