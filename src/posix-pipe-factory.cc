/*
   Copyright (c) 2021 Andreas Fett. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.
*/

#include <unistd.h>
#include <fcntl.h>

#include "posix/pipe-factory.h"
#include "posix/fd.h"
#include "posix/system-error.h"

namespace Posix {

class PipeFactoryImpl : public PipeFactory {
public:
	static std::unique_ptr<PipeFactory> create();
	Pipe make_pipe(Params const&) const override;
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

Pipe PipeFactoryImpl::make_pipe(Params const& params) const
{
	auto pipefd = std::array<int, 2>{-1, -1};
	auto res = ::pipe2(pipefd.data(), pipe_params(params));
	if (res == -1) {
		throw make_system_error(errno, Fmt::format("::pipe2(%x, %s);",
				pipefd.data(), pipe_params(params)));
	}
	return {Fd::create(pipefd[0]), Fd::create(pipefd[1])};
}

}
