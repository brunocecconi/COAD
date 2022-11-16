
/** \file TypeInfo.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_TYPE_INFO_H
#define CORE_TYPE_INFO_H

#include "Core/Common.h"
#include "Core/Assert.h"
#include "Core/Ptr.h"
#include "Core/Allocator.h"

#include <EASTL/functional.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/set.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/string_hash_map.h>

namespace Meta
{

struct TypeInfo
{
	using IdType   = Uint64;
	using SizeType = Uint64;

public:
	EXPLICIT TypeInfo(IdType id, SizeType size, const char* name);
	TypeInfo(const TypeInfo&);
	TypeInfo(TypeInfo&&) NOEXCEPT;
	TypeInfo& operator=(const TypeInfo&);
	TypeInfo& operator=(TypeInfo&&) NOEXCEPT;
	~TypeInfo();

public:
	NODISCARD IdType	  Id() const;
	NODISCARD SizeType	  Size() const;
	NODISCARD const char* Name() const;

public:
	NODISCARD bool operator==(const TypeInfo& left) const;
	NODISCARD bool operator!=(const TypeInfo& left) const;

public:
	template<typename Allocator = EASTLAllocatorType>
	NODISCARD eastl::basic_string<char, Allocator> ToString(const Allocator& allocator = NAME_VAL("TypeInfo")) const;

private:
	IdType		id_;
	SizeType	size_;
	const char* name_;
};

template<typename Allocator> eastl::basic_string<char, Allocator> TypeInfo::ToString(const Allocator& allocator) const
{
	char l_str[512];
	sprintf(l_str, "TypeInfo(name=%s,id=%llu,Size=%llu)", name_, id_, size_);
	return eastl::basic_string<char, Allocator>{l_str, allocator};
}

template<typename T> const TypeInfo& Type();

#define TYPE_INFO_DEFINE(NAME)                                                                                         \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	template<> FORCEINLINE const Meta::TypeInfo& Type<NAME>()                                                          \
	{                                                                                                                  \
		static Meta::TypeInfo l_value{__COUNTER__, sizeof(NAME), #NAME};                                               \
		return l_value;                                                                                                \
	}                                                                                                                  \
	}

} // namespace Meta

TYPE_INFO_DEFINE(bool);
TYPE_INFO_DEFINE(Int8);
TYPE_INFO_DEFINE(Int16);
TYPE_INFO_DEFINE(Int32);
TYPE_INFO_DEFINE(Int64);
TYPE_INFO_DEFINE(Uint8);
TYPE_INFO_DEFINE(Uint16);
TYPE_INFO_DEFINE(Uint32);
TYPE_INFO_DEFINE(Uint64);
TYPE_INFO_DEFINE(Float32);
TYPE_INFO_DEFINE(Float64);

TYPE_INFO_DEFINE(eastl::string);
TYPE_INFO_DEFINE(eastl::wstring);
TYPE_INFO_DEFINE(eastl::string16);
TYPE_INFO_DEFINE(eastl::string32);
TYPE_INFO_DEFINE(eastl::u8string);

TYPE_INFO_DEFINE(eastl::vector<bool>);
TYPE_INFO_DEFINE(eastl::vector<Int8>);
TYPE_INFO_DEFINE(eastl::vector<Int16>);
TYPE_INFO_DEFINE(eastl::vector<Int32>);
TYPE_INFO_DEFINE(eastl::vector<Int64>);
TYPE_INFO_DEFINE(eastl::vector<Uint8>);
TYPE_INFO_DEFINE(eastl::vector<Uint16>);
TYPE_INFO_DEFINE(eastl::vector<Uint32>);
TYPE_INFO_DEFINE(eastl::vector<Uint64>);
TYPE_INFO_DEFINE(eastl::vector<Float32>);
TYPE_INFO_DEFINE(eastl::vector<Float64>);

TYPE_INFO_DEFINE(eastl::vector<eastl::string>);
TYPE_INFO_DEFINE(eastl::vector<eastl::wstring>);
TYPE_INFO_DEFINE(eastl::vector<eastl::string16>);
TYPE_INFO_DEFINE(eastl::vector<eastl::string32>);
TYPE_INFO_DEFINE(eastl::vector<eastl::u8string>);

TYPE_INFO_DEFINE(eastl::set<bool>);
TYPE_INFO_DEFINE(eastl::set<Int8>);
TYPE_INFO_DEFINE(eastl::set<Int16>);
TYPE_INFO_DEFINE(eastl::set<Int32>);
TYPE_INFO_DEFINE(eastl::set<Int64>);
TYPE_INFO_DEFINE(eastl::set<Uint8>);
TYPE_INFO_DEFINE(eastl::set<Uint16>);
TYPE_INFO_DEFINE(eastl::set<Uint32>);
TYPE_INFO_DEFINE(eastl::set<Uint64>);
TYPE_INFO_DEFINE(eastl::set<Float32>);
TYPE_INFO_DEFINE(eastl::set<Float64>);

TYPE_INFO_DEFINE(eastl::set<eastl::string>);
TYPE_INFO_DEFINE(eastl::set<eastl::wstring>);
TYPE_INFO_DEFINE(eastl::set<eastl::string16>);
TYPE_INFO_DEFINE(eastl::set<eastl::string32>);
TYPE_INFO_DEFINE(eastl::set<eastl::u8string>);

namespace eastl
{
template<> struct hash<Meta::TypeInfo>
{
	bool operator()(const Meta::TypeInfo& value) const
	{
		return hash<Uint64>()(value.Id());
	}
};
} // namespace eastl

#endif
