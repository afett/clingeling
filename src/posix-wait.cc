#include "posix/wait.h"

#include <sys/wait.h>

namespace Posix {
namespace Wait {

bool Status::exited() const
{
	return WIFEXITED(value_);
}

int Status::exit_status() const
{
	return WEXITSTATUS(value_);
}

bool Status::signaled() const
{
	return WIFSIGNALED(value_);
}

int Status::termsig() const
{
	return WTERMSIG(value_);
}

bool Status::coredump() const
{
	return WCOREDUMP(value_);
}

bool Status::stopped() const
{
	return WIFSTOPPED(value_);
}

int Status::stopsig() const
{
	return WSTOPSIG(value_);
}

bool Status::continued() const
{
	return WIFCONTINUED(value_);
}

}}
