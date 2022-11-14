
#ifndef CORE_TYPE_TRAITS_H
#define CORE_TYPE_TRAITS_H

#include <EASTL/type_traits.h>
#include <EASTL/tuple.h>

namespace TypeTraits
{

template < typename ... Types >
struct TypeList
{
	static constexpr u64 SIZE = sizeof...(Types);
};

template < typename T1, typename T2>
struct TlCat;

template < typename ... Types1, typename ... Types2>
struct TlCat<TypeList<Types1...>, TypeList<Types2...>>
{
	using Type = TypeList<Types1..., Types2...>;
};

template < typename T >
struct TlToTuple
{
	using Type = eastl::tuple<T>;
};

template < typename ... Types >
struct TlToTuple<TypeList<Types...>>
{
	using Type = eastl::tuple<Types...>;
};

template < typename ... TypeList>
struct ecs
{
	typename TlToTuple<typename TlCat<TypeList...>::Type>::Type tuple;
};

	void asd()
	{
		using T1 = TypeList<int, float, char*>;
		using T2 = TypeList<float, void*>;
		ecs<T1, T2> e{};

		static_assert(eastl::is_same_v<eastl::remove_reference_t<decltype(eastl::get<0>(e.tuple))>, int>);
		static_assert(eastl::is_same_v<eastl::remove_reference_t<decltype(eastl::get<1>(e.tuple))>, float>);
		static_assert(eastl::is_same_v<eastl::remove_reference_t<decltype(eastl::get<2>(e.tuple))>, char*>);
		static_assert(eastl::is_same_v<eastl::remove_reference_t<decltype(eastl::get<3>(e.tuple))>, float>);
		static_assert(eastl::is_same_v<eastl::remove_reference_t<decltype(eastl::get<4>(e.tuple))>, void*>);

		eastl::get<0>(e.tuple);
		eastl::get<1>(e.tuple);
		eastl::get<2>(e.tuple);
		eastl::get<3>(e.tuple);
		eastl::get<4>(e.tuple);
	}

namespace Detail
{

template < typename NewType, typename AnotherType >
struct TlPushFront
{
	using Type = TypeList<NewType, AnotherType>;
};

template < typename NewType, typename ... Types >
struct TlPushFront<NewType, TypeList<Types...>>
{
	using Type = TypeList<NewType, Types...>;
};

template < typename NewType, typename AnotherType >
struct TlPushBack
{
	using Type = TypeList<NewType, AnotherType>;
};

template < typename NewType, typename ... Types >
struct TlPushBack<NewType, TypeList<Types...>>
{
	using Type = TypeList<Types..., NewType>;
};

}

template < typename NewType, typename TypeList >
using TlPushFront = typename Detail::TlPushFront<NewType, TypeList>::Type;

template < typename NewType, typename TypeList >
using TlPushBack = typename Detail::TlPushBack<NewType, TypeList>::Type;

template < typename NewType, typename TypeList >
using TlPushBack = typename Detail::TlPushBack<NewType, TypeList>::Type;

template < typename, typename T >
struct TypeRepeat
{
	using Type = T;
};

template < u64 Index, typename T, typename Tuple >
static constexpr u64 findTupleTypeLogic()
{
	if constexpr (Index < eastl::tuple_size<Tuple>::value)
	{
		if constexpr (eastl::is_same<typename eastl::tuple_element<Index, Tuple>::type, T>::value)
		{
			return Index;
		}
		else
		{
			return findTupleTypeLogic<Index + 1, T, Tuple>();
		}
	}
	else
	{
		return -1;
	}
}

template < typename T, typename Tuple >
static constexpr u64 findTupleType()
{
	return findTupleTypeLogic<0, T, Tuple>();
}

}

#endif
