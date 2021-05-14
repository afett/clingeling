#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "utest/source-location.h"
#include "utest/type-name.h"

namespace UTest {

class Registry {
public:
	static Registry & get();

	void register_test(std::string const&, SourceLocation const&, std::function<void(void)> const&);

	struct Test {
		std::string name;
		SourceLocation loc;
		std::function<void(void)> run;
	};

	using const_iterator = std::vector<Test>::const_iterator;

	const_iterator begin() const;
	const_iterator end() const;

private:
	std::vector<Test> tests_;
};

template <typename T>
class Registrator {
public:
	Registrator(SourceLocation loc)
	{
		Registry::get().register_test(type_name<T>(), loc, []() {
			auto test = std::make_unique<T>();
			(*test)();
		});
	}
};

}
