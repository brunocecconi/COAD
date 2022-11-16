
/** \file Algorithm.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_ALGORITHM_H
#define CORE_ALGORITHM_H

#include "Core/types.h"

template < typename ForwardIterator >
Uint64 MaxElementIndex(ForwardIterator first, ForwardIterator last)
{
	if(first != last)
	{
		ForwardIterator l_current_max = first;
		Uint64 l_index{};
		while(++first != last)
		{
			if(*l_current_max < *first)
			{
				l_current_max = first;
				++l_index;
			}
		}
		return l_index;
	}
	return 0ull;
}

#endif
