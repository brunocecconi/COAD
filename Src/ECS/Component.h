
/** @file Component.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include <EASTL/string.h>

#include "Core/Common.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define ECS_COMPONENT_BODY(NAME)	\
public:	\
CLASS_BODY_ONLY_HEADER(NAME)	\
struct EcsComponentType;

#define ECS_COMPONENT_VALIDATION(NAME)	\
/*TYPE_INFO_DEFINE(NAME);*/	\
static_assert(!eastl::is_polymorphic_v<NAME>, \
"The component class cannot be polymorphic.");

#ifndef COMPONENT_ID_TYPE
#define COMPONENT_ID_TYPE	u64
#endif

namespace Ecs
{

template < typename T, typename = void >
struct IsComponent
{
	static constexpr bool VALUE = false;
};

template < typename T >
struct IsComponent<T, eastl::void_t<typename T::EcsComponentType>>
{
	static constexpr bool VALUE = true;
};

namespace Detail
{

template < Uint64 Index, typename Tuple >
void ValidateComponentTuple()
{
	if constexpr(Index < eastl::tuple_size_v<Tuple>)
	{
		using TupleElementType = eastl::tuple_element_t<Index, Tuple>;
		static_assert(IsComponent<TupleElementType>::VALUE, 
			"Invalid ecs component. You must include ECS_COMPONENT_BODY in component type.");
		ValidateComponentTuple<Index+1, Tuple>();
	}
}

}

template < typename Tuple >
void ValidateComponentTuple()
{
	Detail::ValidateComponentTuple<0, Tuple>();
}

struct LocationComponent
{
	ECS_COMPONENT_BODY(LocationComponent);
	ALIGNAS(16) glm::vec3 value{};
};

struct ALIGNAS(16) RotationComponent
{
	ECS_COMPONENT_BODY(RotationComponent);
	ALIGNAS(16) glm::vec3 value{};

	glm::quat toQuat() const
	{
		return glm::quat{ value };
	}
};

struct ALIGNAS(16) ScaleComponent
{
	ECS_COMPONENT_BODY(ScaleComponent);
	ALIGNAS(16) glm::vec3 value{1.f};
};

struct ALIGNAS(16) Location2dComponent
{
	ECS_COMPONENT_BODY(LocationComponent);
	ALIGNAS(16) glm::vec2 value{};
};

struct ALIGNAS(16) Rotation2dComponent
{
	ECS_COMPONENT_BODY(RotationComponent);
	ALIGNAS(16) glm::vec2 value{};

	glm::quat toQuat() const
	{
		return glm::quat{ glm::vec3{value.x, value.y, 0.f} };
	}
};

struct ALIGNAS(16) Scale2dComponent
{
	ECS_COMPONENT_BODY(ScaleComponent);
	ALIGNAS(16) glm::vec2 value{1.f};
};

struct ALIGNAS(16) NameComponent
{
	ECS_COMPONENT_BODY(NameComponent);
	ALIGNAS(16) eastl::string value{};
};

struct PlaceholderComponent0
{
};
struct PlaceholderComponent1
{
};
struct PlaceholderComponent2
{
};
struct PlaceholderComponent3
{
};
struct PlaceholderComponent4
{
};
struct PlaceholderComponent5
{
};
struct PlaceholderComponent6
{
};
struct PlaceholderComponent7
{
};
struct PlaceholderComponent8
{
};
struct PlaceholderComponent9
{
};
struct PlaceholderComponent10
{
};
struct PlaceholderComponent11
{
};
struct PlaceholderComponent12
{
};
struct PlaceholderComponent13
{
};
struct PlaceholderComponent14
{
};
struct PlaceholderComponent15
{
};
struct PlaceholderComponent16
{
};
struct PlaceholderComponent17
{
};
struct PlaceholderComponent18
{
};
struct PlaceholderComponent19
{
};
struct PlaceholderComponent20
{
};
struct PlaceholderComponent21
{
};
struct PlaceholderComponent22
{
};
struct PlaceholderComponent23
{
};
struct PlaceholderComponent24
{
};
struct PlaceholderComponent25
{
};
struct PlaceholderComponent26
{
};
struct PlaceholderComponent27
{
};
struct PlaceholderComponent28
{
};
struct PlaceholderComponent29
{
};
struct PlaceholderComponent30
{
};
struct PlaceholderComponent31
{
};
struct PlaceholderComponent32
{
};
struct PlaceholderComponent33
{
};
struct PlaceholderComponent34
{
};
struct PlaceholderComponent35
{
};
struct PlaceholderComponent36
{
};
struct PlaceholderComponent37
{
};
struct PlaceholderComponent38
{
};
struct PlaceholderComponent39
{
};
struct PlaceholderComponent40
{
};
struct PlaceholderComponent41
{
};
struct PlaceholderComponent42
{
};
struct PlaceholderComponent43
{
};
struct PlaceholderComponent44
{
};
struct PlaceholderComponent45
{
};
struct PlaceholderComponent46
{
};
struct PlaceholderComponent47
{
};
struct PlaceholderComponent48
{
};
struct PlaceholderComponent49
{
};
struct PlaceholderComponent50
{
};
struct PlaceholderComponent51
{
};
struct PlaceholderComponent52
{
};
struct PlaceholderComponent53
{
};
struct PlaceholderComponent54
{
};
struct PlaceholderComponent55
{
};
struct PlaceholderComponent56
{
};
struct PlaceholderComponent57
{
};
struct PlaceholderComponent58
{
};
struct PlaceholderComponent59
{
};
struct PlaceholderComponent60
{
};
struct PlaceholderComponent61
{
};
struct PlaceholderComponent62
{
};
struct PlaceholderComponent63
{
};

using TranformComponentTypes = TypeTraits::TypeList<
	LocationComponent, RotationComponent, ScaleComponent
	>;
using Tranform2dComponentTypes = TypeTraits::TypeList<
	Location2dComponent, Rotation2dComponent, Scale2dComponent
	>;

using Placeholder32ComponentTypes = TypeTraits::TypeList<
	PlaceholderComponent0,PlaceholderComponent1,PlaceholderComponent2,PlaceholderComponent3,
	PlaceholderComponent4,PlaceholderComponent5,PlaceholderComponent6,PlaceholderComponent7,
	PlaceholderComponent8,PlaceholderComponent9,PlaceholderComponent10,PlaceholderComponent11,
	PlaceholderComponent12,PlaceholderComponent13,PlaceholderComponent14,PlaceholderComponent15,
	PlaceholderComponent16,PlaceholderComponent17,PlaceholderComponent18,PlaceholderComponent19,
	PlaceholderComponent20,PlaceholderComponent21,PlaceholderComponent22,PlaceholderComponent23,
	PlaceholderComponent24,PlaceholderComponent25,PlaceholderComponent26,PlaceholderComponent27,
	PlaceholderComponent28,PlaceholderComponent29,PlaceholderComponent30,PlaceholderComponent31
	>;

using Placeholder64ComponentTypes = TypeTraits::TypeList<
	PlaceholderComponent0,PlaceholderComponent1,PlaceholderComponent2,PlaceholderComponent3,
	PlaceholderComponent4,PlaceholderComponent5,PlaceholderComponent6,PlaceholderComponent7,
	PlaceholderComponent8,PlaceholderComponent9,PlaceholderComponent10,PlaceholderComponent11,
	PlaceholderComponent12,PlaceholderComponent13,PlaceholderComponent14,PlaceholderComponent15,
	PlaceholderComponent16,PlaceholderComponent17,PlaceholderComponent18,PlaceholderComponent19,
	PlaceholderComponent20,PlaceholderComponent21,PlaceholderComponent22,PlaceholderComponent23,
	PlaceholderComponent24,PlaceholderComponent25,PlaceholderComponent26,PlaceholderComponent27,
	PlaceholderComponent28,PlaceholderComponent29,PlaceholderComponent30,PlaceholderComponent31,
	PlaceholderComponent32,PlaceholderComponent33,PlaceholderComponent34,PlaceholderComponent35,
	PlaceholderComponent36,PlaceholderComponent37,PlaceholderComponent38,PlaceholderComponent39,
	PlaceholderComponent40,PlaceholderComponent41,PlaceholderComponent42,PlaceholderComponent43,
	PlaceholderComponent44,PlaceholderComponent45,PlaceholderComponent46,PlaceholderComponent47,
	PlaceholderComponent48,PlaceholderComponent49,PlaceholderComponent50,PlaceholderComponent51,
	PlaceholderComponent52,PlaceholderComponent33,PlaceholderComponent54,PlaceholderComponent55,
	PlaceholderComponent56,PlaceholderComponent57,PlaceholderComponent58,PlaceholderComponent59,
	PlaceholderComponent60,PlaceholderComponent61,PlaceholderComponent62,PlaceholderComponent63
	>;

}

ECS_COMPONENT_VALIDATION(Ecs::LocationComponent);
ECS_COMPONENT_VALIDATION(Ecs::RotationComponent);
ECS_COMPONENT_VALIDATION(Ecs::ScaleComponent);
ECS_COMPONENT_VALIDATION(Ecs::Location2dComponent)
ECS_COMPONENT_VALIDATION(Ecs::Rotation2dComponent);
ECS_COMPONENT_VALIDATION(Ecs::Scale2dComponent);

#endif
