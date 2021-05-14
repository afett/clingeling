#include "utest/macros.h"

#include "json/types.h"

namespace unittests {
namespace json_types {

UTEST_CASE(simple_test)
{
	auto v = Json::Value{"foo"};
	UTEST_ASSERT(Json::holds_alternative<std::string>(v));

	auto o = Json::Object{
		{"foo", Json::Value{"bar"}}
	};

	auto o2 = Json::make_object({
		{"foo", "bar"},
		{"foo", int64_t(1)},
		{"foo", true},
		{"foo", nullptr}
	});
}

UTEST_CASE(get_if_test)
{
	auto v = Json::Value{};
	if (auto res = Json::get_if<nullptr_t>(&v)) {
		UTEST_ASSERT(nullptr == *res);
	} else {
		UTEST_ASSERT(false);
	}
}

UTEST_CASE(get_test)
{
	auto v = Json::Value{};
	auto res = Json::get<nullptr_t>(v);
	UTEST_ASSERT(nullptr == res);
}

UTEST_CASE(visit_test)
{
	auto v = Json::Value{};
	std::visit([](auto) {}, v.get());
}

}}
