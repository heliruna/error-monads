#pragma once
#include "expected.h"
#include <tuple>

namespace uart {

template<typename X, typename TupleT, typename V = void>
struct HasFacetS : public std::false_type {
	
};


template<typename X, typename TupleT>
struct HasFacetS<X,TupleT,std::void_t< decltype(std::get<X>(std::declval<TupleT>())) >> : public std::true_type {
	
};

template<typename X, typename TupleT>
constexpr inline bool hasFacet = HasFacetS<X, TupleT>::value;

template<typename... Types>
struct Monad {
	std::tuple<Types...> data;

	template<typename T>
	explicit Monad(T&& d) : data(std::forward<T>(d)) {}
#if 0
	template<typename X>
	requires hasFacet<X,std::tuple<Types...>>
	operator X&() {
		return std::get<X>(data);
	}

	template<typename X>
	requires hasFacet<X,std::tuple<Types...>>
	operator const X&() const {
		return std::get<X>(data);
	}
    #endif
};

template<bool isConst,typename... Types>
struct MonadView {
	std::tuple<Types...> data;

	explicit MonadView(Monad<Types...>& d) : data(d.data) {}

	template<typename X>
	requires hasFacet<X,std::tuple<Types...>>
	operator X&() {
		return std::get<X>(data);
	}

	template<typename X>
	requires hasFacet<X,std::tuple<Types...>>
	operator const X&() const {
		return std::get<X>(data);
	}
};

template<typename... Types>
struct MonadView<true,Types...> {
	const std::tuple<Types...>& data;

	explicit MonadView(const Monad<Types...>& d) : data(d.data) {}

	template<typename X>
	requires hasFacet<X,std::tuple<Types...>>
	operator const X&() const {
		return std::get<X>(data);
	}

    template<typename X,typename Y>
	requires hasFacet<X,std::tuple<Types...>> and hasFacet<Y,std::tuple<Types...>>
	operator std::tuple<X,Y>() const {
		return std::make_tuple(std::get<X>(data),std::get<Y>(data));
	}

    template<typename X,typename Y>
	requires hasFacet<X,std::tuple<Types...>> and hasFacet<Y,std::tuple<Types...>>
	operator std::tuple<const X&, const Y&>() const {
		return std::make_tuple(std::get<X>(data),std::get<Y>(data));
	}
};




template<typename... Types>
Monad(const std::tuple<Types...>&) -> Monad<Types...>;

template<typename... Types>
Monad(std::tuple<Types...>&&) -> Monad<Types...>;

template<typename... Types>
MonadView(Monad<Types...>&) -> MonadView<false,Types...>;

template<typename... Types>
MonadView(const Monad<Types...>&) -> MonadView<true,Types...>;


template<typename X, typename... Types>
requires (not hasFacet<X,Monad<Types...>>)
Monad<Types...,X> combine(const X& x, const Monad<Types...>& m) {
	return Monad(std::tuple_cat(m.data,std::tuple<X>(x)));
}

template<typename X, typename... Types>
requires (not hasFacet<X,Monad<Types...>>)
Monad<Types...,X> combine(const Monad<Types...>& m, const X& x) {
	return combine(x,m);
}

template<typename X, typename... Types>
requires (hasFacet<X,Monad<Types...>>)
Monad<Types...> combine(const Monad<Types...>& m, const X& x) {
	auto r = m;
	get<X>(r) = x;
	return r;
}

template<typename X, typename... Types>
requires (hasFacet<X,Monad<Types...>>)
Monad<Types...> operator and(const X& x, const Monad<Types...>& m) {
	return combine(m,x);
}

template<typename X, typename... Types, typename Error>
auto combine(const expected<Monad<Types...>,Error>& m, const expected<X,Error>& x) {
	using XMonad = decltype(combine(*m,*x));
	using EMonad = expected<XMonad,Error>;
	if (bool(m) and bool(x)) {
		return EMonad(combine(*m, *x));
	} else if (x) {
		return EMonad(unexpected(m.error()));
	} else {
        return EMonad(unexpected(x.error()));
    }
}

template<typename X, typename... Types, typename Error>
auto combine(const expected<X,Error>& x, const expected<Monad<Types...>,Error>& m) {
	return combine(m,x);
}


template< class T, class... Types >
constexpr T& get( Monad<Types...>& t ) noexcept {
	return std::get<T>(t.data);
}

template< class T, class... Types >
constexpr const T& get( const Monad<Types...>& t ) noexcept {
	return std::get<T>(t.data);
}

template< class T, class... Types >
constexpr T&& get( Monad<Types...>&& t ) noexcept {
	return std::move(std::get<T>(t.data));
}

}