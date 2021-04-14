#include <cppunit/extensions/HelperMacros.h>

#define CPPUNIT_ASSERT_THROW_VAR(expression, ExceptionType, var)           \
do {                                                                       \
	bool cpputCorrectExceptionThrown_ = false;                         \
	CPPUNIT_NS::Message cpputMsg_("expected exception not thrown");    \
	cpputMsg_.addDetail(CPPUNIT_NS::AdditionalMessage());              \
	cpputMsg_.addDetail("Expected: "                                   \
		CPPUNIT_GET_PARAMETER_STRING(ExceptionType));              \
	try {                                                              \
		expression;                                                \
	} catch (ExceptionType const& e) {                                 \
		cpputCorrectExceptionThrown_ = true;                       \
		var = e;                                                   \
	} catch (std::exception const& e) {                                \
		cpputMsg_.addDetail("Actual  : " +                         \
			CPPUNIT_EXTRACT_EXCEPTION_TYPE_(e,                 \
				"std::exception or derived"));             \
		cpputMsg_.addDetail(std::string("What()  : ") + e.what()); \
	} catch (...) {                                                    \
		cpputMsg_.addDetail("Actual  : unknown.");                 \
	}                                                                  \
	                                                                   \
	if (cpputCorrectExceptionThrown_) {                                \
		break;                                                     \
	}		                                                   \
	CPPUNIT_NS::Asserter::fail(cpputMsg_, CPPUNIT_SOURCELINE());       \
} while (false)
