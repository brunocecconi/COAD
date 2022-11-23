
#ifndef CORE_HASH_H
#define CORE_HASH_H

#include "Core/BaseTypes.h"

namespace Hash
{

using fnv1a_t = Uint64;

// See http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
constexpr fnv1a_t FNV_BASIS = 14695981039346656037ull;
constexpr fnv1a_t FNV_PRIME = 1099511628211ull;

// FNV-1a 64 bit hash
constexpr fnv1a_t Fnv1AHash(const Size n, const char * str, const fnv1a_t hash = FNV_BASIS)
{
    return n > 0 ? Fnv1AHash(n - 1, str + 1, (hash ^ *str) * FNV_PRIME) : hash;
}

template<Size N>
constexpr fnv1a_t Fnv1AHash(const char (&array)[N])
{
    return Fnv1AHash(N - 1, &array[0]);
}

namespace Literals
{

constexpr fnv1a_t operator"" _fnv1(const char* str, const Size length)
{
    return Fnv1AHash(length, str);
}

}

}

#endif
