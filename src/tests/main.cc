#include "utest/assert.h"
#include "utest/registry.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char *argv[])
{
	auto list{false};
	std::string testname;
	int opt;

	while ((opt = getopt(argc, argv, "lt:")) != -1) {
		switch (opt) {
		case 'l':
			list = true;
			break;
		case 't':
			testname = optarg;
			break;
		default: /* '?' */
			std::cerr <<  "Usage:" << argv[0] << " [-t name] [-l]\n";
			return EXIT_FAILURE;
		}
	}

	if (list) {
		for (auto const& test : UTest::Registry::get()) {
			std::cout << test.name << "\n";
		}
		return EXIT_SUCCESS;
	}

	size_t total{0};
	size_t failed{0};
	for (auto const& test : UTest::Registry::get()) {
		if (!testname.empty() && test.name.find(testname) == 0) {
			continue;
		}

		++total;
		try {
			test.run();
			std::cout << "[OK] " << test.name << "\n";
			continue;
		} catch (UTest::AssertionFailure const& fail) {
			std::cerr << fail.loc << " Test " << test.name << " failed:\n";
			std::cerr << fail.loc << " " << fail.msg << "\n";
		} catch (std::exception const& e) {
			std::cerr << test.loc << " Test " << test.name << " failed:\n";
			std::cerr << test.loc << " unhandled exception of type " << UTest::type_name(e) << ": what: " << e.what() << "\n";
		} catch (...) {
			std::cerr << test.loc << " Test " << test.name << " failed:\n";
			std::cerr << test.loc << " unhandled excepetion of unknown type \n";
		}
		std::cout << "[FAIL] " << test.name << "\n";
		++failed;
	}

	std::cout << (failed == 0 ? "[OK]" : "[FAILED]") << " " << total << " Tests: OK:" << total - failed << " FAILED: " << failed << "\n";
	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
