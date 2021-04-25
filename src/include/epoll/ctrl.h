#pragma once

#include <chrono>
#include <functional>
#include <memory>

namespace Posix {
class Fd;
}

namespace EPoll {

class Event;

class Ctrl {
public:
	virtual void add(std::shared_ptr<Posix::Fd> const&, Event const&, std::function<void(Event const&)> const&) = 0;
	virtual void del(std::shared_ptr<Posix::Fd> const&) = 0;
	virtual void mod(std::shared_ptr<Posix::Fd> const&, Event const&) const = 0;

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
