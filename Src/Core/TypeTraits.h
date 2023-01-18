
/** @file TypeTraits.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_TYPE_TRAITS_H
#define CORE_TYPE_TRAITS_H

#include "Core/Compiler.h"

#include <EASTL/type_traits.h>
#include <EASTL/tuple.h>

namespace TypeTraits
{

template<typename... Types>
struct TypeList
{
	static constexpr uint64_t SIZE = sizeof...(Types);
};

template<typename TypeList>
struct TlGetFirstElement;

template<typename Head, typename... Tail>
struct TlGetFirstElement<TypeList<Head, Tail...>>
{
	using type_t = Head;
};

template<typename TypeList, uint64_t Index>
struct TlGetByIndex;

template<typename Head, typename... Tail>
struct TlGetByIndex<TypeList<Head, Tail...>, 0>
{
	using type_t = Head;
};

template<typename Head, typename... Tail, uint64_t N>
struct TlGetByIndex<TypeList<Head, Tail...>, N>
{
	using type_t = typename TlGetByIndex<TypeList<Tail...>, N - 1>::type_t;
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

template<typename T, uint64_t Size>
struct TlCreateTypeList
{
	using type_t = typename TlPushFront<T, typename TlCreateTypeList<T, Size - 1>::type_t>::type_t;
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
template<uint64_t Index, typename T, typename Tuple>
static constexpr uint64_t FindTupleType()
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
static constexpr uint64_t FindTupleType()
{
	static_assert(TlHasType<T, Tuple>::value, "Type does not contain in tuple.");
	return Detail::FindTupleType<0, T, Tuple>();
}

template<typename Function>
struct FunctionTraits;

template<typename R, typename... Args>
struct FunctionTraits<R (*)(Args...)>
{
	using owner_t			= void;
	using return_t			= R;
	using param_type_list_t = TypeList<Args...>;
};

template<typename R, typename... Args>
struct FunctionTraits<R(Args...)>
{
	using owner_t			= void;
	using return_t			= R;
	using param_type_list_t = TypeList<Args...>;
};

template<typename Owner, typename R, typename... Args>
struct FunctionTraits<R (Owner::*)(Args...)>
{
	using owner_t			= Owner;
	using return_t			= R;
	using param_type_list_t = TypeList<Args...>;
};

template<typename Owner, typename R, typename... Args>
struct FunctionTraits<R (Owner::*)(Args...) const>
{
	using owner_t			= Owner;
	using return_t			= R;
	using param_type_list_t = TypeList<Args...>;
};

template<typename T>
struct PropertyTraits;

template<typename Owner, typename T>
struct PropertyTraits<T(Owner::*)>
{
	using owner_t = Owner;
	using type_t  = T;
};

/**
 * @brief Has const iterator.
 *
 * @link https://stackoverflow.com/questions/9407367/determine-if-a-type-is-an-stl-container-at-compile-time
 *
 * @tparam T Type.
 *
 */
template<typename T>
struct HasConstIterator
{
private:
	typedef char yes_t;
	typedef struct
	{
		char array[2];
	} no_t;

	template<typename C>
	static yes_t Test(typename C::const_iterator *);
	template<typename C>
	static no_t Test(...);

public:
	static const bool VALUE = sizeof(Test<T>(nullptr)) == sizeof(yes_t);
	typedef T		  type_t;
};

/**
 * @brief
 *
 * @link https://stackoverflow.com/questions/9407367/determine-if-a-type-is-an-stl-container-at-compile-time
 *
 * @tparam T Type.
 *
 */
template<typename T>
struct HasBeginEnd
{
	template<typename C>
	static char (
		&F(std::enable_if_t<std::is_same_v<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
										   typename C::const_iterator (C::*)() const>,
							void> *))[1];

	template<typename C>
	static char (&F(...))[2];

	template<typename C>
	static char (
		&G(std::enable_if_t<std::is_same_v<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
										   typename C::const_iterator (C::*)() const>,
							void> *))[1];

	template<typename C>
	static char (&G(...))[2];

	static bool const BEG_VALUE = sizeof(F<T>(nullptr)) == 1;
	static bool const END_VALUE = sizeof(G<T>(nullptr)) == 1;
};

/**
 * @brief Is container.
 *
 * @link https://stackoverflow.com/questions/9407367/determine-if-a-type-is-an-stl-container-at-compile-time
 *
 * @tparam T Type.
 *
 */
template<typename T>
struct IsContainer
{
	static constexpr bool							   VALUE = eastl::integral_constant < bool,
						  HasConstIterator<T>::value &&HasBeginEnd<T>::beg_value &&HasBeginEnd<T>::end_value > ::value;
};

} // namespace TypeTraits

#endif
