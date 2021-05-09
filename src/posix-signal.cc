#include "posix/signal.h"

#include <cstring>

namespace Posix {

std::string to_string(Signal sig)
{
	return ::strsignal(static_cast<int>(sig));
}

}
