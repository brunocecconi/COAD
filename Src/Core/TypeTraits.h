
#ifndef CORE_TYPE_TRAITS_H
#define CORE_TYPE_TRAITS_H

#include <EASTL/type_traits.h>
#include <EASTL/tuple.h>
#include <EASTL/array.h>

namespace TypeTraits
{

template < typename ... Types >
struct TypeList
{
	static constexpr u64 SIZE = sizeof...(Types);
};

using TypeListEmpty = TypeList<>;

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

template < template<typename> typename T, typename Y >
struct TlToTupleTransfer
{
	using Type = eastl::tuple<T<Y>>;
};

template < template<typename> typename T, typename ... Types >
struct TlToTupleTransfer<T, TypeList<Types...>>
{
	using Type = eastl::tuple<T<Types>...>;
};

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

template < typename T, u64 Size >
struct TlCreateTypeList
{
	using Type = TlPushFront<T, typename TlCreateTypeList<T, Size-1>::Type>;
};

template < typename T >
struct TlCreateTypeList<T, 0>
{
	using Type = TypeList<>;
};

template < typename, typename T >
struct TypeRepeat
{
	using Type = T;
};

template < typename Tuple >
constexpr auto TupleToArray(Tuple&& tuple)
{
	constexpr auto l_array = []<typename... Args>(Args&&... x) { return eastl::array{std::forward<Args>(x) ... }; };
	return eastl::apply(l_array, std::forward<Tuple>(tuple));
}

template <typename T, typename Tuple>
struct TlHasType;

template <typename T>
struct TlHasType<T, eastl::tuple<>> : std::false_type {};

template <typename T, typename U, typename... Ts>
struct TlHasType<T, eastl::tuple<U, Ts...>> : TlHasType<T, eastl::tuple<Ts...>> {};

template <typename T, typename... Ts>
struct TlHasType<T, eastl::tuple<T, Ts...>> : eastl::true_type {};

namespace Detail
{
template < u64 Index, typename T, typename Tuple >
static constexpr u64 FindTupleType()
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
}

template < typename T, typename Tuple >
static constexpr u64 FindTupleType()
{
	static_assert(TlHasType<T, Tuple>::value, "Type does not contain in tuple.");
	return Detail::FindTupleType<0, T, Tuple>();
}

}

#endif
