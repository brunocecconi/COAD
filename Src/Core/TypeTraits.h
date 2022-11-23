
/** @file TypeTraits.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_TYPE_TRAITS_H
#define CORE_TYPE_TRAITS_H

#include <string_view>

#include "Core/Compiler.h"

#include <EASTL/type_traits.h>
#include <EASTL/tuple.h>
#include <EASTL/array.h>
#include <EASTL/string_view.h>
#include <EASTL/bitset.h>

namespace TypeTraits
{

template<typename... Types>
struct TypeList
{
	static constexpr Uint64 SIZE = sizeof...(Types);
};

template < typename TypeList >
struct TlGetFirstElement;

template<typename Head, typename... Tail>
struct TlGetFirstElement<TypeList<Head, Tail...>>
{
	using type_t = Head;
};

template < typename TypeList, Uint64 Index >
struct TlGetByIndex;

template<typename Head, typename... Tail>
struct TlGetByIndex<TypeList<Head, Tail...>, 0>
{
	using type_t = Head;
};

template<typename Head, typename... Tail, Uint64 N>
struct TlGetByIndex<TypeList<Head, Tail...>, N>
{
	using type_t = typename TlGetByIndex<TypeList<Tail...>, N-1>::type_t;
};

using type_list_empty_t = TypeList<>;

template<typename T1, typename T2>
struct TlCat;

template<typename... Types1, typename... Types2>
struct TlCat<TypeList<Types1...>, TypeList<Types2...>>
{
	using type_t = TypeList<Types1..., Types2...>;
};

template<typename T>
struct TlToTuple
{
	using type_t = eastl::tuple<T>;
};

template<typename... Types>
struct TlToTuple<TypeList<Types...>>
{
	using type_t = eastl::tuple<Types...>;
};

template<template<typename> typename T, typename Y>
struct TlToTupleTransfer
{
	using type_t = eastl::tuple<T<Y>>;
};

template<template<typename> typename T, typename... Types>
struct TlToTupleTransfer<T, TypeList<Types...>>
{
	using type_t = eastl::tuple<T<Types>...>;
};

template<typename NewType, typename AnotherType>
struct TlPushFront
{
	using type_t = TypeList<NewType, AnotherType>;
};

template<typename NewType, typename... Types>
struct TlPushFront<NewType, TypeList<Types...>>
{
	using type_t = TypeList<NewType, Types...>;
};

template<typename NewType, typename AnotherType>
struct TlPushBack
{
	using type_t = TypeList<NewType, AnotherType>;
};

template<typename NewType, typename... Types>
struct TlPushBack<NewType, TypeList<Types...>>
{
	using type_t = TypeList<Types..., NewType>;
};

template<typename T, Uint64 Size>
struct TlCreateTypeList
{
	using type_t = TlPushFront<T, typename TlCreateTypeList<T, Size - 1>::Type>;
};

template<typename T>
struct TlCreateTypeList<T, 0>
{
	using type_t = TypeList<>;
};

template<typename, typename T>
struct TypeRepeat
{
	using type_t = T;
};

template<typename Tuple>
constexpr auto TupleToArray(Tuple&& tuple)
{
	constexpr auto l_array = []<typename... Args>(Args&&... x) { return eastl::array{std::forward<Args>(x)...}; };
	return eastl::apply(l_array, std::forward<Tuple>(tuple));
}

template<typename T, typename Tuple>
struct TlHasType;

template<typename T>
struct TlHasType<T, eastl::tuple<>>: std::false_type
{
};

template<typename T, typename U, typename... Ts>
struct TlHasType<T, eastl::tuple<U, Ts...>>: TlHasType<T, eastl::tuple<Ts...>>
{
};

template<typename T, typename... Ts>
struct TlHasType<T, eastl::tuple<T, Ts...>>: eastl::true_type
{
};

namespace Detail
{
template<Uint64 Index, typename T, typename Tuple>
static constexpr Uint64 FindTupleType()
{
	if constexpr (Index < eastl::tuple_size<Tuple>::value)
	{
		if constexpr (eastl::is_same<typename eastl::tuple_element<Index, Tuple>::type, T>::value)
		{
			return Index;
		}
		else
		{
			return FindTupleType<Index + 1, T, Tuple>();
		}
	}
}

} // namespace Detail

template<typename T, typename Tuple>
static constexpr Uint64 FindTupleType()
{
	static_assert(TlHasType<T, Tuple>::value, "Type does not contain in tuple.");
	return Detail::FindTupleType<0, T, Tuple>();
}

template < typename Function >
struct FunctionTraits;

template < typename R, typename ... Args>
struct FunctionTraits<R(Args...)>
{
	using return_t = R;
	using param_type_list_t = TypeList<Args...>;
};

template < typename T >
struct PropertyTraits;

template < typename Owner, typename T >
struct PropertyTraits<T (Owner::*)>
{
	using owner_t = Owner;
	using type_t = T;
};

} // namespace TypeTraits

#endif
