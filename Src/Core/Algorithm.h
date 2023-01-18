
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

template<uint32_t Index, uint32_t Size, typename T>
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

template<typename TFunction, typename... TArgs>
void Call(TFunction Function, TArgs&&... Args)
{
	return Function(eastl::forward<TArgs>(Args)...);
}

template<typename TFunction, typename... TArgs>
void Call(eastl::vector<TFunction>& Functions, TArgs&&... Args)
{
	eastl::for_each(Functions.begin(), Functions.end(), [&](TFunction& f) { f(eastl::forward<TArgs>(Args)...); });
}

template<typename ForwardIterator>
uint64_t MaxElementIndex(ForwardIterator First, ForwardIterator Last)
{
	if (First != Last)
	{
		ForwardIterator lCurrentMax = First;
		uint64_t		lIndex{};
		while (++First != Last)
		{
			if (*lCurrentMax < *First)
			{
				lCurrentMax = First;
				++lIndex;
			}
		}
		return lIndex;
	}
	return 0ull;
}

template<typename ForwardIterator>
uint64_t MinElementIndex(ForwardIterator First, ForwardIterator Last)
{
	if (First != Last)
	{
		ForwardIterator lCurrentMin = First;
		uint64_t		lIndex{};
		while (++First != Last)
		{
			if (*lCurrentMin > *First)
			{
				lCurrentMin = First;
				++lIndex;
			}
		}
		return lIndex;
	}
	return 0ull;
}

template<size_t Index, typename TTuple>
constexpr void CtContainerTupleReserve(TTuple& Tuple, size_t NewCapacity)
{
	if constexpr (Index < eastl::tuple_size_v<TTuple>)
	{
		eastl::get<Index>(Tuple).reserve(NewCapacity);
		CtContainerTupleReserve<Index + 1>(Tuple, NewCapacity);
	}
}

} // namespace Algorithm

#endif
