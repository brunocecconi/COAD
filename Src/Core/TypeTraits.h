
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
struct FunctionTraits<R(*)(Args...)>
{
	using owner_t = void;
	using return_t = R;
	using param_type_list_t = TypeList<Args...>;
};

template < typename R, typename ... Args>
struct FunctionTraits<R(Args...)>
{
	using owner_t = void;
	using return_t = R;
	using param_type_list_t = TypeList<Args...>;
};

template < typename Owner, typename R, typename ... Args>
struct FunctionTraits<R(Owner::*)(Args...)>
{
	using owner_t = Owner;
	using return_t = R;
	using param_type_list_t = TypeList<Args...>;
};

template < typename Owner, typename R, typename ... Args>
struct FunctionTraits<R(Owner::*)(Args...) const>
{
	using owner_t = Owner;
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

class conststr
{
    public:
        template<std::size_t N>
        constexpr conststr(const char(&STR)[N])
        :string(STR), size(N-1)
        {}

        constexpr conststr(const char* STR, std::size_t N)
        :string(STR), size(N)
        {}

        constexpr char operator[](std::size_t n)
        {
            return n < size ? string[n] : 0;
        }

        constexpr std::size_t get_size()
        {
            return size;
        }

        constexpr const char* get_string()
        {
            return string;
        }

        //This method is related with Fowler–Noll–Vo hash function
        constexpr Uint64 hash(int n=0, unsigned h=2166136261)
        {
            return n == size ? h : hash(n+1,(h * 16777619) ^ (string[n]));
        }

    private:
        const char* string;
        std::size_t size;
};

Uint64 constexpr operator "" _const(const char* str, size_t sz)
{
    return conststr(str,sz).hash();
}

template < Uint64 Id >
struct Rebinder;


} // namespace TypeTraits

#endif
