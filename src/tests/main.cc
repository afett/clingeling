#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestResult.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>
#include <cstdlib>
#include <unistd.h>

namespace {

void printTestTree(CppUnit::Test const* root)
{
	std::cerr << root->getName() << "\n";
	for (int i(0); i < root->getChildTestCount(); ++i) {
		printTestTree(root->getChildTestAt(i));
	}
}

}

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

	CppUnit::TestFactoryRegistry & registry(
		CppUnit::TestFactoryRegistry::getRegistry());
	CppUnit::Test *test(registry.makeTest());

	if (list) {
		printTestTree(test);
		return EXIT_SUCCESS;
	}

	CppUnit::TextTestRunner runner;
	runner.addTest(test);

	CppUnit::BriefTestProgressListener progress;
	runner.eventManager().addListener(&progress);

	runner.setOutputter(new CppUnit::CompilerOutputter(
		&runner.result(), std::cerr, "%p:%l:"));

	return runner.run(testname) ? EXIT_SUCCESS : EXIT_FAILURE;
}
