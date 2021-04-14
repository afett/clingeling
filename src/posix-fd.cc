#include <unistd.h>

#include "posix/fd.h"

namespace Posix {

Fd::Priv::~Priv()
{
	if (fd != -1) {
		::close(fd);
	}
}

}
