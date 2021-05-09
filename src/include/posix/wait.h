#pragma once

namespace Posix {
namespace Wait {

class Status {
public:
	Status() = default;
	Status(Status const&) = default;
	Status & operator=(Status const&) = default;
	explicit Status(int value)
	:
		value_{value}
	{ }

	bool exited() const;
	int exit_status() const;
	bool signaled() const;
	int termsig() const;
	bool coredump() const;
	bool stopped() const;
	int stopsig() const;
	bool continued() const;

private:
	int value_ = 0;
};

}}

