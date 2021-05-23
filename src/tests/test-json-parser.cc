#include "utest/macros.h"

#include "json/parser.h"
#include "assertion-traits.h"

namespace unittests {
namespace json_parser {

UTEST_CASE(empty_object_test)
{
	{
		std::stringstream is{"{}"};
		auto o = Json::parse_object(is);
		UTEST_ASSERT(o.empty());
	}

	{
		std::stringstream is{" {}"};
		auto o = Json::parse_object(is);
		UTEST_ASSERT(o.empty());
	}

	{
		std::stringstream is{"{} "};
		auto o = Json::parse_object(is);
		UTEST_ASSERT(o.empty());
	}

	{
		std::stringstream is{"{ }"};
		auto o = Json::parse_object(is);
		UTEST_ASSERT(o.empty());
	}

	{
		std::stringstream is{" { } "};
		auto o = Json::parse_object(is);
		UTEST_ASSERT(o.empty());
	}

	{
		std::stringstream is{"    {        }     "};
		auto o = Json::parse_object(is);
		UTEST_ASSERT(o.empty());
	}
}

UTEST_CASE(simple_object_test)
{
	std::stringstream is{
		"{"
			"\"foo\": \"bar\""
		"}"
	};
	auto o = Json::parse_object(is);
	UTEST_ASSERT(!o.empty());
	UTEST_ASSERT_EQUAL(size_t(1), o.size());
	UTEST_ASSERT(Json::holds_alternative<std::string>(o["foo"]));
	UTEST_ASSERT_EQUAL(std::string{"bar"}, Json::get<std::string>(o["foo"]));
}

UTEST_CASE(empty_array_test)
{
	{
		std::stringstream is{"[]"};
		auto a = Json::parse_array(is);
		UTEST_ASSERT(a.empty());
	}

	{
		std::stringstream is{" []"};
		auto a = Json::parse_array(is);
		UTEST_ASSERT(a.empty());
	}

	{
		std::stringstream is{"[] "};
		auto a = Json::parse_array(is);
		UTEST_ASSERT(a.empty());
	}

	{
		std::stringstream is{"[ ]"};
		auto a = Json::parse_array(is);
		UTEST_ASSERT(a.empty());
	}

	{
		std::stringstream is{" [ ] "};
		auto a = Json::parse_array(is);
		UTEST_ASSERT(a.empty());
	}

	{
		std::stringstream is{"    [        ]     "};
		auto o = Json::parse_array(is);
		UTEST_ASSERT(o.empty());
	}
}

UTEST_CASE(simple_array_test)
{
	std::stringstream is{
		"["
			"\"bar\""
		"]"
	};
	auto a = Json::parse_array(is);
	UTEST_ASSERT(!a.empty());
	UTEST_ASSERT_EQUAL(size_t(1), a.size());
	UTEST_ASSERT(Json::holds_alternative<std::string>(a[0]));
	UTEST_ASSERT_EQUAL(std::string{"bar"}, Json::get<std::string>(a[0]));
}

UTEST_CASE(bool_test)
{
	std::stringstream is{
		"{"
			"\"foo\": true,"
			"\"bar\": false"
		"}"
	};
	auto o = Json::parse_object(is);
	UTEST_ASSERT(!o.empty());
	UTEST_ASSERT_EQUAL(size_t(2), o.size());
	UTEST_ASSERT(Json::holds_alternative<bool>(o["foo"]));
	UTEST_ASSERT_EQUAL(true, Json::get<bool>(o["foo"]));
	UTEST_ASSERT(Json::holds_alternative<bool>(o["bar"]));
	UTEST_ASSERT_EQUAL(false, Json::get<bool>(o["bar"]));
}

UTEST_CASE(null_test)
{
	std::stringstream is{
		"{"
			"\"foo\": null"
		"}"
	};
	auto o = Json::parse_object(is);
	UTEST_ASSERT(!o.empty());
	UTEST_ASSERT_EQUAL(size_t(1), o.size());
	UTEST_ASSERT(Json::holds_alternative<nullptr_t>(o["foo"]));
	UTEST_ASSERT_EQUAL(nullptr, Json::get<nullptr_t>(o["foo"]));
}

UTEST_CASE(number_test)
{
	std::stringstream is{
		"["
			"0, +0, -0, 1, +1, -1"
		"]"
	};
	auto a = Json::parse_array(is);
	UTEST_ASSERT(!a.empty());
	UTEST_ASSERT_EQUAL(size_t(6), a.size());
	UTEST_ASSERT(Json::holds_alternative<int64_t>(a[0]));
	UTEST_ASSERT_EQUAL(int64_t(0), Json::get<int64_t>(a[0]));
	UTEST_ASSERT(Json::holds_alternative<int64_t>(a[1]));
	UTEST_ASSERT_EQUAL(int64_t(0), Json::get<int64_t>(a[1]));
	UTEST_ASSERT(Json::holds_alternative<int64_t>(a[2]));
	UTEST_ASSERT_EQUAL(int64_t(0), Json::get<int64_t>(a[2]));
	UTEST_ASSERT(Json::holds_alternative<int64_t>(a[3]));
	UTEST_ASSERT_EQUAL(int64_t(1), Json::get<int64_t>(a[3]));
	UTEST_ASSERT(Json::holds_alternative<int64_t>(a[4]));
	UTEST_ASSERT_EQUAL(int64_t(1), Json::get<int64_t>(a[4]));
	UTEST_ASSERT(Json::holds_alternative<int64_t>(a[5]));
	UTEST_ASSERT_EQUAL(int64_t(-1), Json::get<int64_t>(a[5]));
}

}}
