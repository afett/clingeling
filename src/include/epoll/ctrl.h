#pragma once

#include "flags.h"

#include <chrono>
#include <functional>
#include <memory>

namespace Posix {
class Fd;
}

namespace EPoll {

enum class Event {
	In = 0,
	Out,
	RdHup,
	Pri,
	Err,
	Hup,
};

using Events = Flags<Event>;

class Ctrl {
public:
	virtual void add(std::shared_ptr<Posix::Fd> const&, Events const&, std::function<void(Events const&)> const&) = 0;
	virtual void del(std::shared_ptr<Posix::Fd> const&) = 0;
	virtual void mod(std::shared_ptr<Posix::Fd> const&, Events const&) const = 0;

	static std::chrono::milliseconds Infinity();
	virtual bool wait(std::chrono::milliseconds const& = Infinity()) const = 0;

	virtual ~Ctrl() = default;
};

class CtrlFactory {
public:
	static std::unique_ptr<CtrlFactory> create();
	virtual std::unique_ptr<Ctrl> make_ctrl() const = 0;
	virtual ~CtrlFactory() = default;
};

}

DECLARE_FLAG_TYPE(EPoll::Event);
