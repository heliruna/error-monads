#pragma once
#include <optional>
#include <stdexcept>
namespace uart {

template <class E>
class bad_expected_access;

template <>
class bad_expected_access<void> : public std::exception {
public:
	const char* what() const noexcept override {
		return "bad expected access";
	}
};

template <class E>
class bad_expected_access : public bad_expected_access<void> {
public:
	bad_expected_access(E e) :
	    error_value(e) {}

	[[nodiscard]] E& error() noexcept { return error_value; }

	[[nodiscard]] const E& error() const noexcept { return error_value; }

private:
	E error_value {};
};

template <typename E>
struct unexpected {
	using error_type = E;

	E error_value {};

	constexpr explicit unexpected(E e) noexcept :
	    error_value(e) {}

	[[nodiscard]] constexpr E& error() noexcept { return error_value; }

	[[nodiscard]] constexpr const E& error() const noexcept {
		return error_value;
	}
};

template <typename E>
unexpected(E) -> unexpected<E>;

template <typename T, typename E>
struct expected {
	using expected_type = T;
	using error_type = E;

	std::optional<T> value_data;
	E error_value {};

	bool has_value() const { return bool(value_data); }

	template <typename G>
	constexpr expected(unexpected<G>&& u) noexcept
	    :
	    error_value(std::move(u.error())) {}

	template <typename G>
	constexpr expected(const unexpected<G>& u) noexcept
	    :
	    error_value(u.error()) {}

	template <typename... Args>
	constexpr expected(Args&&... args) noexcept
	    :
	    value_data(std::forward<Args>(args)...) {}

	explicit operator bool() const { return has_value(); }

	template <class U>
	constexpr T value_or(U&& default_value) const& {
		return bool(*this) ? **this
		                   : static_cast<T>(std::forward<U>(default_value));
	}

	template <class U>
	constexpr T value_or(U&& default_value) && {
		return bool(*this) ? std::move(**this)
		                   : static_cast<T>(std::forward<U>(default_value));
	}

	constexpr T& value() & {
		if (bool(this)) {
			return operator*();
		} else {
			throw bad_expected_access(std::as_const(error()));
		}
	}

	constexpr const T& value() const& {
		if (bool(this)) {
			return operator*();
		} else {
			throw bad_expected_access(error());
		}
	}

	constexpr T&& value() && {
		if (bool(this)) {
			return operator*();
		} else {
			throw bad_expected_access(std::move(error()));
		}
	}

	constexpr T& operator*() & noexcept { return *value_data; }
	constexpr const T& operator*() const& noexcept { return *value_data; }
	constexpr T&& operator*() && noexcept { return *value_data; }

	constexpr T* operator->() noexcept { return value_data.operator->(); }
	constexpr const T* operator->() const noexcept { return value_data.operator->(); }

	[[nodiscard]] constexpr E& error() noexcept { return error_value; }

	[[nodiscard]] constexpr const E& error() const noexcept { return error_value; }
};

}
