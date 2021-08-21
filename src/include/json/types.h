/*
   Copyright (c) 2021 Andreas Fett
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include <vector>
#include <map>
#include <string>
#include <variant>

namespace Json {

class Value;

using Array = std::vector<Value>;
using Object = std::map<std::string, Value>;

class Value {
public:
	using value_type = std::variant<nullptr_t, bool, uint64_t, int64_t, std::string, Object, Array>;

	Value() = default;

	explicit Value(nullptr_t)
	:
		value_{nullptr}
	{ }

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

	value_type get() const
	{
		return value_;
	}

	template <typename T> friend std::add_pointer_t<T> get_if(Value *);
	template <typename T> friend std::add_pointer_t<const T> get_if(Value const*);
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

template <typename T>
std::add_pointer_t<T> get_if(Value * v)
{
	return std::get_if<T>(&v->value_);
}

template <typename T>
std::add_pointer_t<const T> get_if(Value const* v)
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
