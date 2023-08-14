#pragma once
#include <cstdint>
#include <stdexcept>

#if __cpp_lib_source_location >= 201907L
#include <source_location>
#endif

namespace uart {

struct StatusCode {
	using value_type = uint32_t;
	value_type value {0};

	constexpr StatusCode() noexcept = default;
	explicit constexpr StatusCode(unsigned status) :
	    value(status) {}
	constexpr explicit operator value_type() const { return value; }
	static inline constexpr value_type errorMask() { return 0x80'00'00'00u; }

	static inline constexpr StatusCode timeout() { return StatusCode(0x80'00'00'03); }
};

constexpr inline bool hasSucceeded(StatusCode code) {
	return (unsigned(code) & StatusCode::errorMask()) == 0u;
}

struct FatalError : public std::runtime_error {
	template <typename Arg>
	FatalError(Arg&& arg) :
	    std::runtime_error(std::forward<Arg>(arg)) {}
};

#if __cpp_lib_source_location >= 201907L
[[noreturn]] void fail(StatusCode sc, std::source_location&& location = std::source_location::current());
[[noreturn]] inline void fail(std::source_location&& location = std::source_location::current()) {
	fail(StatusCode(0x80'00'00'00), std::move(location));
}
template <typename A>
inline void ensure_equal(A actual, A expected, std::source_location&& location = std::source_location::current()) {
	if (actual != expected) {
		fail(StatusCode(0x80'00'00'00), std::move(location));
	}
}
#else
[[noreturn]] void fail(StatusCode sc);
[[noreturn]] inline void fail() {
	fail(StatusCode(0x80'00'00'00));
}
template <typename A>
inline void ensure_equal(A actual, A expected) {
	if (actual != expected) {
		fail(StatusCode(0x80'00'00'00));
	}
}

#endif

std::string to_string(StatusCode sc);

}