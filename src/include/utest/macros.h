#pragma once

#include <exception>

#include "utest/assert.h"
#include "utest/registry.h"

#define UTEST_ASSERT(expr) \
	UTest::assert((expr), std::string{ #expr }, {__FILE__, __LINE__})

#define UTEST_ASSERT_EQUAL(expected, result) \
	UTest::assert_equal((expected), (result), std::string{ #expected }, std::string{ #result }, {__FILE__, __LINE__})

#define UTEST_ASSERT_THROW(expr, exception) \
	try {   auto utest_failure_location = UTest::SourceLocation{__FILE__, __LINE__};         \
		(expr);                                                     \
		throw UTest::AssertionFailure{std::string("Expected ") + #expr + " to throw exception of type " + #exception, utest_failure_location}; \
	} catch (exception const& e) {                                      \
	}

#define UTEST_CASE(name)                                        \
class name {                                                   \
public:                                                        \
	void operator()();                                     \
};                                                             \
auto name ## Registrator = UTest::Registrator<name>{{__FILE__, __LINE__}}; \
void name::operator()()

#define UTEST_CASE_WITH_FIXTURE(name, fixture)                  \
class name : public fixture {                                  \
public:                                                        \
	void operator()();                                     \
};                                                             \
auto name ## Registrator = UTest::Registrator<name>{{__FILE__, __LINE__}};  \
void name::operator()()
