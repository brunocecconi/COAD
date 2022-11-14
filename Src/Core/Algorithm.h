
#ifndef CORE_ALGORITHM_H
#define CORE_ALGORITHM_H

#include "Core/types.h"

template < typename ForwardIterator >
u64 maxElementIndex(ForwardIterator first, ForwardIterator last)
{
	if(first != last)
	{
		ForwardIterator l_current_max = first;
		u64 l_index{};
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
