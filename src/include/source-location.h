#pragma once

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
