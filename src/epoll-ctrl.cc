#include <cassert>

#include <sys/epoll.h>

#include "epoll/ctrl.h"
#include "epoll/event.h"
#include "posix/fd.h"
#include "posix/system-error.h"

namespace EPoll {

std::chrono::milliseconds Ctrl::Infinity()
{
	return std::chrono::milliseconds::max();
}

class CtrlImpl : public Ctrl {
public:
	CtrlImpl();

	void add(std::shared_ptr<Posix::Fd> const&, Event const&, std::function<void(Event const&)> const&) override;
	void del(std::shared_ptr<Posix::Fd> const&) override;
	void mod(std::shared_ptr<Posix::Fd> const&, Event const&) const override;
	bool wait(std::chrono::milliseconds const&) const override;

private:

	struct Callback {
		std::shared_ptr<Posix::Fd> fd;
		std::function<void(Event const&)> fn;
	};

	std::unordered_map<int, std::unique_ptr<Callback>> cb_;
	std::shared_ptr<Posix::Fd> fd_;
};

}

namespace {

uint32_t epoll_events(EPoll::Event const& ev)
{
	uint32_t res{0};
	if (ev & EPoll::Event::Type::In) {
		res |= EPOLLIN;
	}
	if (ev & EPoll::Event::Type::Out) {
		res |= EPOLLOUT;
	}
	if (ev & EPoll::Event::Type::RdHup) {
		res |= EPOLLRDHUP;
	}
	if (ev & EPoll::Event::Type::Pri) {
		res |= EPOLLPRI;
	}
	if (ev & EPoll::Event::Type::Err) {
		res |= EPOLLERR;
	}
	if (ev & EPoll::Event::Type::Hup) {
		res |= EPOLLHUP;
	}
	return res;
}

EPoll::Event epoll_events(uint32_t ev)
{
	EPoll::Event res{};
	if (ev & EPOLLIN) {
		res |= EPoll::Event::Type::In;
	}
	if (ev & EPOLLOUT) {
		res |= EPoll::Event::Type::Out;
	}
	if (ev & EPOLLRDHUP) {
		res |= EPoll::Event::Type::RdHup;
	}
	if (ev & EPOLLPRI) {
		res |= EPoll::Event::Type::Pri;
	}
	if (ev & EPOLLERR) {
		res |= EPoll::Event::Type::Err;
	}
	if (ev & EPOLLHUP) {
		res |= EPoll::Event::Type::Hup;
	}
	return res;
}

}

namespace EPoll {

CtrlImpl::CtrlImpl()
:
	fd_(Posix::Fd::create(::epoll_create1(EPOLL_CLOEXEC)))
{
	if (fd_->get() == -1) {
		throw Posix::make_system_error(errno, "::epoll_create1(EPOLL_CLOEXEC)");
	}
}

void CtrlImpl::add(std::shared_ptr<Posix::Fd> const& fd, Event const& ev, std::function<void(Event const&)> const& fn)
{
	auto cb = std::unique_ptr<Callback>(new Callback{fd, fn});
	epoll_event epoll_ev;
	epoll_ev.events = ::epoll_events(ev);
	epoll_ev.data.ptr = cb.get();
	if (::epoll_ctl(fd_->get(), EPOLL_CTL_ADD, fd->get(), &epoll_ev) == -1) {
		// throw Posix::make_system_error(errno, "::epoll_ctl(%s, EPOLL_CTL_ADD, %s, &epoll_ev)", fd_.get(), fd.get());
		throw POSIX_SYSTEM_ERROR("::epoll_ctl(%s, EPOLL_CTL_ADD, %s, &epoll_ev)", fd_->get(), fd->get());
	}
	cb_.emplace(fd->get(), std::move(cb));
}

void CtrlImpl::del(std::shared_ptr<Posix::Fd> const& fd)
{
	if (cb_.erase(fd->get()) == 0) {
		throw std::runtime_error("cound not find fd to delete");
	}
	if (::epoll_ctl(fd_->get(), EPOLL_CTL_DEL, fd->get(), nullptr) == -1) {
		throw Posix::make_system_error(errno, "::epoll_ctl(%s, EPOLL_CTL_DEL, %s, nullptr)", fd_->get(), fd->get());
	}
}

void CtrlImpl::mod(std::shared_ptr<Posix::Fd> const& fd, Event const& ev) const
{
	auto it{cb_.find(fd->get())};
	if (it == std::end(cb_)) {
		throw std::runtime_error(Fmt::format("could not find fd %s to modify", fd->get()));
	}

	epoll_event epoll_ev;
	epoll_ev.events = ::epoll_events(ev);
	epoll_ev.data.ptr = it->second.get();
	if (::epoll_ctl(fd_->get(), EPOLL_CTL_MOD, fd->get(), &epoll_ev) == -1) {
		throw Posix::make_system_error(errno, "::epoll_ctl(%s, EPOLL_CTL_MOD, %s, &epoll_ev)", fd_->get(), fd->get());
	}
}

bool CtrlImpl::wait(std::chrono::milliseconds const& timeout = Infinity()) const
{
	int to = -1;
	if (timeout != Infinity()) {
		to = timeout.count();
	}

	auto events = std::array<epoll_event, 10>{};
	int nevents{-1};
	do {
		nevents = ::epoll_wait(fd_->get(), events.data(), events.size(), to);
	} while (nevents == -1 && errno == EINTR);

	if (nevents == -1) {
		throw Posix::make_system_error(errno, "::epoll_wait(%s, events.data(), %s, %s)", fd_.get(), events.size(), to);
	}

	assert(nevents >= 0);
	for (size_t n{0}; n < size_t(nevents); ++n) {
		auto cb = static_cast<Callback*>(events[n].data.ptr);
		cb->fn(::epoll_events(events[n].events));
	}

	return nevents != 0;
}

class CtrlFactoryImpl : public CtrlFactory {
public:
	std::unique_ptr<Ctrl> make_ctrl() const override;
};

std::unique_ptr<CtrlFactory> CtrlFactory::create()
{
	return std::make_unique<CtrlFactoryImpl>();
}

std::unique_ptr<Ctrl> CtrlFactoryImpl::make_ctrl() const
{
	return std::make_unique<CtrlImpl>();
}

}
