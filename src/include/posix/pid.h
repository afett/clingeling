#pragma once

#include <memory>

namespace Posix {

enum class Signal;

namespace Wait {
class Status;
enum class Option;
}

class Pid {
public:
	static std::shared_ptr<Pid> create(int);
	virtual void kill(Signal) = 0;
	virtual Wait::Status wait(Wait::Option) = 0;

	virtual ~Pid() = default;
};

}
