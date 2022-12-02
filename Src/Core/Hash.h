
#ifndef CORE_HASH_H
#define CORE_HASH_H

#include "Core/BaseTypes.h"

namespace Hash
{

using fnv1a_t = Uint64;

// See http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
static constexpr fnv1a_t FNV_BASIS = 146959810346656037ll;
static constexpr fnv1a_t FNV_PRIME = 1099511628211ll;

// FNV-1a 64 bit hash
static constexpr fnv1a_t Fnv1A(const Size n, const char * str, const fnv1a_t hash = FNV_BASIS)
{
    return n > 0 ? Fnv1A(n - 1, str + 1, (hash ^ *str) * FNV_PRIME) : hash;
}

template<Size N>
static constexpr fnv1a_t Fnv1A(const char (&array)[N])
{
    return Fnv1A(N - 1, &array[0]);
}


}

static constexpr Hash::fnv1a_t operator"" _fnv1a(const char* str, const Size length)
{
    return Hash::Fnv1A(length, str);
}

#define FNV1(STRING_LITERAL)    Hash::Fnv1AHash(STRING_LITERAL)

#endif
