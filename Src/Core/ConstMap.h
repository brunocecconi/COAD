
#ifndef CORE_CONST_MAP_H
#define CORE_CONST_MAP_H

#include "Core/Compiler.h"
#include "Core/BaseTypes.h"
#include "Core/Assert.h"

#include <EASTL/array.h>
#include <EASTL/utility.h>

template < typename Key, typename Value, size_t Size >
struct ConstMap
{
	eastl::array<eastl::pair<Key, Value>, Size> data;

	NODISCARD constexpr Value At(const Key& key) const
	{
		const auto l_it = eastl::find_if(data.begin(), data.end(),
			[&key](const auto& value) { return value.first == key; } );
		ENFORCE_MSG(l_it != data.end(), "Value not found!");
		return l_it->second;
	}
};

#endif
