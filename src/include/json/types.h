#pragma once

#include <vector>
#include <map>
#include <variant>

namespace Json {

class Value;

using Array = std::vector<Value>;
using Object = std::map<std::string, Value>;

class Value {
public:
	using value_type = std::variant<nullptr_t, bool, uint64_t, int64_t, std::string, Object, Array>;

	Value() = default;

	explicit Value(bool v)
	:
		value_{v}
	{ }

	explicit Value(uint64_t v)
	:
		value_{v}
	{ }

	explicit Value(int64_t v)
	:
		value_{v}
	{ }

	explicit Value(std::string const& v)
	:
		value_{v}
	{ }

	explicit Value(const char* v)
	:
		value_{std::string{v}}
	{ }

	explicit Value(Object const& v)
	:
		value_{v}
	{ }

	explicit Value(Array const& v)
	:
		value_{v}
	{ }

	bool is_object() const
	{
		return std::holds_alternative<Object>(value_);
	}

	template <typename T> friend std::add_pointer_t<T> get_if(Value *);
	template <class T> friend T& get(Value &);
	template <class T> friend T&& get(Value &&);
	template <class T> friend T const& get(Value const&);
	template <class T> friend T const&& get(Value const&&);
	template <class T> friend bool holds_alternative(Value const&);

private:
	value_type value_{nullptr};

};

class ValueMaker {
public:
	ValueMaker(nullptr_t)
	:
		value_{}
	{ }

	ValueMaker(bool v)
	:
		value_{v}
	{ }

	ValueMaker(uint64_t v)
	:
		value_{v}
	{ }

	ValueMaker(int64_t v)
	:
		value_{v}
	{ }

	ValueMaker(std::string const& v)
	:
		value_{v}
	{ }

	ValueMaker(const char* v)
	:
		value_{std::string{v}}
	{ }

	ValueMaker(Object const& v)
	:
		value_{v}
	{ }

	ValueMaker(Array const& v)
	:
		value_{v}
	{ }

private:
	friend Object make_object(std::initializer_list<std::pair<std::string, ValueMaker>>);
	friend Array make_array(std::initializer_list<ValueMaker>);

	Value value_{};
};

inline Object make_object(std::initializer_list<std::pair<std::string, ValueMaker>> l)
{
	auto res =  Object{};
	for (auto const& m : l) {
		res.emplace(std::move(m.first), std::move(m.second.value_));
	}
	return res;
}

inline Array make_array(std::initializer_list<ValueMaker> l)
{
	auto res = Array{};
	for (auto const& e : l) {
		res.push_back(std::move(e.value_));
	}
	return res;
}

/*
template <class Visitor>
std::invoke_result_t<Visitor, Value> visit(Visitor && vis, Value && v)
{
	return std::visit(std::forward<Visitor>(vis), std::forward<Value>(v));
}
*/

template <typename T>
std::add_pointer_t<T> get_if(Value *v)
{
	return std::get_if<T>(&v->value_);
}

template <class T>
T& get(Value & v)
{
	return std::get<T>(v.value_);
}

template <class T>
T&& get(Value && v)
{
	return std::get<T>(v.value_);
}

template <class T>
T const& get(Value const& v)
{
	return std::get<T>(v.value_);
}

template <class T>
T const&& get(Value const&& v)
{
	return std::get<T>(v.value_);
}

template<class T>
bool holds_alternative(Value const& v)
{
	return std::holds_alternative<T>(v.value_);
}

}
