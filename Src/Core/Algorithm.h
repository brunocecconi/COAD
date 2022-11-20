
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
#include "Core/Meta.h"

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

		PrintArray<Index + 1>(arr);
	}
}

} // namespace Detail

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

/**
 * @brief Prints an array.
 *
 * @tparam T Target type.
 * @tparam Size Target size.
 * @param arr Target array.
 *
 */
template<typename T, Uint32 Size>
constexpr void PrintArray(const eastl::array<T, Size>& arr)
{
	printf("eastl::array<%s, %u>( ", Meta::Detail::TypeName<T>().data(), Size);
	Detail::PrintArray<0>(arr);
	printf(")\n");
}

} // namespace Algorithm

#endif
