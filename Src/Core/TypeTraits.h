
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

namespace TypeTraits
{

namespace Detail
{
template <Size... Indices>
constexpr auto SubstringAsArray(std::string_view str, eastl::index_sequence<Indices...>)
{
  return eastl::array{str[Indices]..., '\n'};
}

template <typename T>
constexpr auto TypeNameArray()
{
#if PLATFORM_WINDOWS
	constexpr auto prefix	= std::string_view{"TypeNameArray<"};
	constexpr auto suffix	= std::string_view{">(void)"};
	constexpr auto function = std::string_view{__PRETTY_FUNCTION__};

	constexpr auto start = function.find(prefix) + prefix.size();
	constexpr auto end	 = function.rfind(suffix);

	static_assert(start < end);

	constexpr auto name = function.substr(start, end-start);

	return SubstringAsArray(name, eastl::make_index_sequence<name.size()>{});
#endif
}

template <typename T>
struct TypeNameHolder
{
	static inline constexpr auto VALUE = TypeNameArray<T>();
};
}

template <typename T>
constexpr eastl::string_view TypeName()
{
  constexpr auto& value = Detail::TypeNameHolder<T>::VALUE;
  return {value.data(), value.size()};
}

template<typename... Types> struct TypeList
{
	static constexpr Uint64 SIZE = sizeof...(Types);
};

using TypeListEmpty = TypeList<>;

template<typename T1, typename T2> struct TlCat;

template<typename... Types1, typename... Types2> struct TlCat<TypeList<Types1...>, TypeList<Types2...>>
{
	using Type = TypeList<Types1..., Types2...>;
};

template<typename T> struct TlToTuple
{
	using Type = eastl::tuple<T>;
};

template<typename... Types> struct TlToTuple<TypeList<Types...>>
{
	using Type = eastl::tuple<Types...>;
};

template<template<typename> typename T, typename Y> struct TlToTupleTransfer
{
	using Type = eastl::tuple<T<Y>>;
};

template<template<typename> typename T, typename... Types> struct TlToTupleTransfer<T, TypeList<Types...>>
{
	using Type = eastl::tuple<T<Types>...>;
};

template<typename NewType, typename AnotherType> struct TlPushFront
{
	using Type = TypeList<NewType, AnotherType>;
};

template<typename NewType, typename... Types> struct TlPushFront<NewType, TypeList<Types...>>
{
	using Type = TypeList<NewType, Types...>;
};

template<typename NewType, typename AnotherType> struct TlPushBack
{
	using Type = TypeList<NewType, AnotherType>;
};

template<typename NewType, typename... Types> struct TlPushBack<NewType, TypeList<Types...>>
{
	using Type = TypeList<Types..., NewType>;
};

template<typename T, Uint64 Size> struct TlCreateTypeList
{
	using Type = TlPushFront<T, typename TlCreateTypeList<T, Size - 1>::Type>;
};

template<typename T> struct TlCreateTypeList<T, 0>
{
	using Type = TypeList<>;
};

template<typename, typename T> struct TypeRepeat
{
	using Type = T;
};

template<typename Tuple> constexpr auto TupleToArray(Tuple&& tuple)
{
	constexpr auto l_array = []<typename... Args>(Args&&... x) { return eastl::array{std::forward<Args>(x)...}; };
	return eastl::apply(l_array, std::forward<Tuple>(tuple));
}

template<typename T, typename Tuple> struct TlHasType;

template<typename T> struct TlHasType<T, eastl::tuple<>>: std::false_type
{
};

template<typename T, typename U, typename... Ts>
struct TlHasType<T, eastl::tuple<U, Ts...>>: TlHasType<T, eastl::tuple<Ts...>>
{
};

template<typename T, typename... Ts> struct TlHasType<T, eastl::tuple<T, Ts...>>: eastl::true_type
{
};

namespace Detail
{
template<Uint64 Index, typename T, typename Tuple> static constexpr Uint64 FindTupleType()
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

template<typename T, typename Tuple> static constexpr Uint64 FindTupleType()
{
	static_assert(TlHasType<T, Tuple>::value, "Type does not contain in tuple.");
	return Detail::FindTupleType<0, T, Tuple>();
}

} // namespace TypeTraits

#endif
