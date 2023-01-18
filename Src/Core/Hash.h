
#ifndef CORE_HASH_H
#define CORE_HASH_H

#include "Core/BaseTypes.h"

namespace Hash
{

using fnv1a_t = uint64_t;

// See http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
static constexpr fnv1a_t FNV_BASIS = 146959810346656037ll;
static constexpr fnv1a_t FNV_PRIME = 1099511628211ll;

// FNV-1a 64 bit hash
static constexpr fnv1a_t Fnv1A(const size_t Length, const char* Str, const fnv1a_t Hash = FNV_BASIS)
{
	return Length > 0 ? Fnv1A(Length - 1, Str + 1, (Hash ^ *Str) * FNV_PRIME) : Hash;
}

template<size_t Length>
static constexpr fnv1a_t Fnv1A(const char (&Array)[Length])
{
	return Fnv1A(Length - 1, &Array[0]);
}

} // namespace Hash

static constexpr Hash::fnv1a_t operator"" _fnv1a(const char* Str, const size_t Length)
{
	return Hash::Fnv1A(Length, Str);
}

#define FNV1(STRING_LITERAL) Hash::Fnv1AHash(STRING_LITERAL)

#endif
