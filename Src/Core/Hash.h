
#ifndef CORE_HASH_H
#define CORE_HASH_H

#include "Core/BaseTypes.h"

namespace Hash
{

using Type = Uint64;

// See http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
constexpr Type FNV_BASIS = 14695981039346656037ull;
constexpr Type FNV_PRIME = 1099511628211ull;

// FNV-1a 64 bit hash
constexpr Type Fnv1AHash(const Size n, const char * str, const Type hash = FNV_BASIS)
{
    return n > 0 ? Fnv1AHash(n - 1, str + 1, (hash ^ *str) * FNV_PRIME) : hash;
}

template<Size N>
constexpr Type Fnv1AHash(const char (&array)[N])
{
    return Fnv1AHash(N - 1, &array[0]);
}

namespace Literals
{

constexpr Type operator"" _fnv1(const char* str, const Size length)
{
    return Fnv1AHash(length, str);
}

}

}

#endif
