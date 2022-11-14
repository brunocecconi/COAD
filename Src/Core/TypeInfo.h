
#ifndef CORE_TYPE_INFO_H
#define CORE_TYPE_INFO_H

#include "core/common.h"
#include "core/assert.h"
#include "core/ptr.h"
#include "core/allocator.h"

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
	using IdType = u64;
	using SizeType = u64;

public:
	EXPLICIT TypeInfo(IdType id, SizeType size, const char* name);
	TypeInfo(const TypeInfo&);
	TypeInfo(TypeInfo&&) NOEXCEPT;
	TypeInfo& operator=(const TypeInfo&);
	TypeInfo& operator=(TypeInfo&&) NOEXCEPT;
	~TypeInfo();

public:
	NODISCARD IdType Id() const;
	NODISCARD SizeType Size() const;
	NODISCARD const char* Name() const;

public:
	NODISCARD bool operator==(const TypeInfo& left) const;
	NODISCARD bool operator!=(const TypeInfo& left) const;

public:
	template < typename Allocator = EASTLAllocatorType >
	NODISCARD eastl::basic_string<char, Allocator> ToString(
		const Allocator& allocator = NAME_VAL("TypeInfo")) const;

private:
	IdType id_;
	SizeType size_;
	const char* name_;
};

template <typename Allocator>
eastl::basic_string<char, Allocator> TypeInfo::ToString(const Allocator& allocator) const
{
	char l_str[512];
	sprintf(l_str, "TypeInfo(name=%s,id=%llu,Size=%llu)", name_, id_, size_);
	return eastl::basic_string<char, Allocator>{ l_str, allocator };
}

template < typename T >
const TypeInfo& type();

#define TYPE_INFO_DEFINE(NAME)	\
namespace Meta{	\
template<> FORCEINLINE const Meta::TypeInfo& type<NAME>()	\
{	\
	static Meta::TypeInfo l_value{__COUNTER__, sizeof(NAME), #NAME};	\
	return l_value;	\
}}

}

TYPE_INFO_DEFINE(bool);
TYPE_INFO_DEFINE(i8);
TYPE_INFO_DEFINE(i16);
TYPE_INFO_DEFINE(i32);
TYPE_INFO_DEFINE(i64);
TYPE_INFO_DEFINE(u8);
TYPE_INFO_DEFINE(u16);
TYPE_INFO_DEFINE(u32);
TYPE_INFO_DEFINE(u64);
TYPE_INFO_DEFINE(f32);
TYPE_INFO_DEFINE(f64);

TYPE_INFO_DEFINE(eastl::string);
TYPE_INFO_DEFINE(eastl::wstring);
TYPE_INFO_DEFINE(eastl::string16);
TYPE_INFO_DEFINE(eastl::string32);
TYPE_INFO_DEFINE(eastl::u8string);

TYPE_INFO_DEFINE(eastl::vector<bool>);
TYPE_INFO_DEFINE(eastl::vector<i8>);
TYPE_INFO_DEFINE(eastl::vector<i16>);
TYPE_INFO_DEFINE(eastl::vector<i32>);
TYPE_INFO_DEFINE(eastl::vector<i64>);
TYPE_INFO_DEFINE(eastl::vector<u8>);
TYPE_INFO_DEFINE(eastl::vector<u16>);
TYPE_INFO_DEFINE(eastl::vector<u32>);
TYPE_INFO_DEFINE(eastl::vector<u64>);
TYPE_INFO_DEFINE(eastl::vector<f32>);
TYPE_INFO_DEFINE(eastl::vector<f64>);

TYPE_INFO_DEFINE(eastl::vector<eastl::string>);
TYPE_INFO_DEFINE(eastl::vector<eastl::wstring>);
TYPE_INFO_DEFINE(eastl::vector<eastl::string16>);
TYPE_INFO_DEFINE(eastl::vector<eastl::string32>);
TYPE_INFO_DEFINE(eastl::vector<eastl::u8string>);

TYPE_INFO_DEFINE(eastl::set<bool>);
TYPE_INFO_DEFINE(eastl::set<i8>);
TYPE_INFO_DEFINE(eastl::set<i16>);
TYPE_INFO_DEFINE(eastl::set<i32>);
TYPE_INFO_DEFINE(eastl::set<i64>);
TYPE_INFO_DEFINE(eastl::set<u8>);
TYPE_INFO_DEFINE(eastl::set<u16>);
TYPE_INFO_DEFINE(eastl::set<u32>);
TYPE_INFO_DEFINE(eastl::set<u64>);
TYPE_INFO_DEFINE(eastl::set<f32>);
TYPE_INFO_DEFINE(eastl::set<f64>);

TYPE_INFO_DEFINE(eastl::set<eastl::string>);
TYPE_INFO_DEFINE(eastl::set<eastl::wstring>);
TYPE_INFO_DEFINE(eastl::set<eastl::string16>);
TYPE_INFO_DEFINE(eastl::set<eastl::string32>);
TYPE_INFO_DEFINE(eastl::set<eastl::u8string>);

namespace eastl
{
	template<>
	struct hash<Meta::TypeInfo>
	{
		bool operator()(const Meta::TypeInfo& value) const
		{
			return hash<u64>()(value.Id());
		}
	};
}

#endif
