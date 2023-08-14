#pragma once
#include "Monad.h"
#include "expected.h"
namespace uart {

template<typename Error,typename X, typename... Types>
auto combine(const expected<X,Error>& x, const expected<Monad<Types...>,Error>& m) {

}

template <typename Callable>
struct Chainable {
	#if 0
	template <typename Value, typename Error>
	auto operator()(const expected<Value, Error>& input) -> decltype(std::declval<Callable&>()(*input)) {
		if (input) {
			return static_cast<Callable&>(*this)(*input);
		} else {
			return unexpected(input.error());
		}
	}
	

	template <typename Value, typename Error>
	auto operator()(const expected<Value, Error>& input) const -> decltype(std::declval<const Callable&>()(*input)) {
		if (input) {
			return static_cast<const Callable&>(*this)(*input);
		} else {
			return unexpected(input.error());
		}
	}
	#endif

	template <typename... Values, typename Error>
	auto operator()(const expected<Monad<Values...>, Error>& input) {
		
		return combine(input, static_cast<Callable&>(*this)( MonadView(*input)));

	}

    template <typename... Values>
	auto operator()(const Monad<Values...>& input) {
        using Error = std::decay_t<decltype(static_cast<Callable&>(*this)( MonadView(input)).error())>;
		return combine(expected<Monad<Values...>,Error>(input), static_cast<Callable&>(*this)( MonadView(input)));

	}


};

}