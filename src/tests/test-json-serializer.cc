#include "utest/macros.h"

#include "json/serializer.h"

namespace unittests {
namespace json_serializer {

UTEST_CASE(serialize_object_test)
{
	auto o = Json::make_object({
		{"null", nullptr},
		{"true", true},
		{"false", false},
		{"uint64_t", uint64_t(42)},
		{"int64_t", int64_t(-42)},
		{"string", "foobar"},
		{"array", Json::make_array({})},
		{"object", Json::make_object({})},
	});

	auto ref = std::string{
		"{"
		"\"array\": [], "
		"\"false\": false, "
		"\"int64_t\": -42, "
		"\"null\": null, "
		"\"object\": {}, "
		"\"string\": \"foobar\", "
		"\"true\": true, "
		"\"uint64_t\": 42"
		"}"
	};

	UTEST_ASSERT_EQUAL(ref, Json::to_string(o));
}

UTEST_CASE(serialize_array_test)
{
	auto o = Json::make_array({
		nullptr,
		true,
		false,
		uint64_t(42),
		int64_t(-42),
		"foobar",
		Json::make_array({}),
		Json::make_object({}),
	});

	auto ref = std::string{
		"["
		"null, "
		"true, "
		"false, "
		"42, "
		"-42, "
		"\"foobar\", "
		"[], "
		"{}"
		"]"
	};

	UTEST_ASSERT_EQUAL(ref, Json::to_string(o));
}

}}
