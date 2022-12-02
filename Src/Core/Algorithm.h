
/** \file Algorithm.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_ALGORITHM_H
#define CORE_ALGORITHM_H

#include "Core/Types.h"
#include "Core/RawBuffer.h"

#include <EASTL/array.h>

namespace Algorithm
{

namespace Detail
{

template<Uint32 Index, Uint32 Size, typename T>
constexpr void PrintArray(const eastl::array<T, Size>& arr)
{
	if constexpr (Index < Size)
	{
		if constexpr (eastl::is_fundamental_v<T>)
		{
			if constexpr (eastl::is_pointer_v<T>)
			{
				if (arr[Index])
				{
					printf("%s ", arr[Index]);
				}
			}
			else
			{
				printf("%s ", arr[Index]);
			}
		}
		else
		{
			if constexpr (eastl::is_pointer_v<T>)
			{
				if (arr[Index])
				{
					printf("%s ", arr[Index]->ToString().c_str());
				}
			}
			else
			{
				printf("%s ", arr[Index].ToString().c_str());
			}
		}

		PrintArray<Index + 1>(arr);
	}
}

} // namespace Detail

template < typename Function, typename ... Args >
void Call(Function function, Args&&... args)
{
	return function(eastl::forward<Args>(args)...);
}

template < typename Function, typename ... Args >
void Call(eastl::vector<Function>& functions,
	Args&&... args)
{
	eastl::for_each(functions.begin(), functions.end(), [&](Function& f) {
		f(eastl::forward<Args>(args)...);
	});
}

template<typename ForwardIterator>
Uint64 MaxElementIndex(ForwardIterator first, ForwardIterator last)
{
	if (first != last)
	{
		ForwardIterator l_current_max = first;
		Uint64			l_index{};
		while (++first != last)
		{
			if (*l_current_max < *first)
			{
				l_current_max = first;
				++l_index;
			}
		}
		return l_index;
	}
	return 0ull;
}

template<typename ForwardIterator>
Uint64 MinElementIndex(ForwardIterator first, ForwardIterator last)
{
	if (first != last)
	{
		ForwardIterator l_current_min = first;
		Uint64			l_index{};
		while (++first != last)
		{
			if (*l_current_min > *first)
			{
				l_current_min = first;
				++l_index;
			}
		}
		return l_index;
	}
	return 0ull;
}

template < Size Index, typename Tuple >
constexpr void CtContainerTupleReserve(Tuple& tuple, Size new_capacity)
{
	if constexpr (Index < eastl::tuple_size_v<Tuple>)
	{
		eastl::get<Index>(tuple).reserve(new_capacity);
		CtContainerTupleReserve<Index+1>(tuple, new_capacity);
	}
}

} // namespace Algorithm

#endif
