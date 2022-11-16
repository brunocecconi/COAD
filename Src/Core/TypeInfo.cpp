
/** @file TypeInfo.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#include "Core/TypeInfo.h"

namespace Meta
{

TypeInfo::TypeInfo(const IdType id, const SizeType size, const char* name) : id_{ id }, size_{ size }, name_{ name }
{
}

TypeInfo::TypeInfo(const TypeInfo&) = default;
TypeInfo::TypeInfo(TypeInfo&&) noexcept = default;
TypeInfo& TypeInfo::operator=(const TypeInfo&) = default;
TypeInfo& TypeInfo::operator=(TypeInfo&&) noexcept = default;
TypeInfo::~TypeInfo() = default;

TypeInfo::IdType TypeInfo::Id() const
{
	return id_;
}

TypeInfo::SizeType TypeInfo::Size() const
{
	return size_;
}

const char* TypeInfo::Name() const
{
	return name_;
}

bool TypeInfo::operator==(const TypeInfo& left) const
{
	return id_ == left.id_;
}

bool TypeInfo::operator!=(const TypeInfo& left) const
{
	return id_ != left.id_;
}

}
