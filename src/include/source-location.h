#pragma once

#include <string>
#include <stdexcept>

class SourceLocation {
public:
	using Line = decltype(__builtin_LINE());
	using File = decltype(__builtin_FILE());
	using Function = decltype(__builtin_FUNCTION());

	static constexpr SourceLocation current(
		Line line_ = __builtin_LINE(),
		File file_ = __builtin_FILE(),
		Function function_ = __builtin_FUNCTION()) noexcept
	{
		return {line_, file_, function_};
	}

	Line line = 0;
	File file = "unknown";
	Function function = "unknown";
};

inline std::string to_string(SourceLocation const& sl)
{
	return std::string{sl.file} + ":" + std::to_string(sl.line) + ": " + sl.function;
}

inline void throw_backtrace(SourceLocation sl = SourceLocation::current())
{
	std::throw_with_nested(std::runtime_error(to_string(sl)));
}
