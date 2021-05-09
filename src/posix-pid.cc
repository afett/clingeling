#include "posix/pid.h"

#include "posix/signal.h"
#include "posix/system-error.h"
#include "posix/wait.h"

#include <signal.h>
#include <sys/wait.h>

namespace Posix {

class PidImpl : public Pid {
public:
	explicit PidImpl(int value)
	:
		value_{value}
	{
		static_assert(std::is_same_v<pid_t, int>);
	}

	void kill(Signal) override;
	Wait::Status wait(Wait::Option) override;

private:
	pid_t value_ = -1;
};

std::shared_ptr<Pid> Pid::create(int value)
{
	return std::make_shared<PidImpl>(value);
}

void PidImpl::kill(Signal sig)
{
	auto res = ::kill(value_, static_cast<int>(sig));
	if (res == -1) {
		throw make_system_error(errno, "::kill(%s, %s)", value_, static_cast<int>(sig));
	}
}

namespace {

int waitpid_opts(Wait::Option opt)
{
	return opt == Wait::Option::NoHang ? WNOHANG : 0;
}

}

Wait::Status PidImpl::wait(Wait::Option opt)
{
	int status{0};
	auto res = ::waitpid(value_, &status, waitpid_opts(opt));
	if (res == -1) {
		throw make_system_error(errno, "::waitpid(%s, %x, %s)", value_, &status, waitpid_opts(opt));
	}
	return Wait::Status{status};
}

}
