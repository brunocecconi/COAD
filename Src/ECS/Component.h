
#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include "Core/Common.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define ECS_COMPONENT_BODY(NAME)	\
public:	\
CLASS_BODY_ONLY_HEADER(NAME)	\
struct EcsComponentType{};

#define ECS_COMPONENT_VALIDATION(NAME)	\
TYPE_INFO_DEFINE(NAME);	\
static_assert(!eastl::is_polymorphic_v<NAME>, \
"The component class cannot be polymorphic.");	\
static_assert(eastl::is_same_v<eastl::void_t<typename NAME::EcsComponentType>, void>, \
"Target type is not an entity component.");

#ifndef COMPONENT_ID_TYPE
#define COMPONENT_ID_TYPE	u64
#endif

namespace Ecs
{

struct LocationComponent
{
	ECS_COMPONENT_BODY(LocationComponent)
	glm::vec3 value{};
};

struct RotationComponent
{
	ECS_COMPONENT_BODY(RotationComponent);
	glm::vec3 value{};

	glm::quat toQuat() const
	{
		return glm::quat{ value };
	}
};

struct ScaleComponent
{
	ECS_COMPONENT_BODY(ScaleComponent);
	glm::vec3 value{1.f};
};

struct Location2dComponent
{
	ECS_COMPONENT_BODY(LocationComponent);
	glm::vec2 value{};
};

struct Rotation2dComponent
{
	ECS_COMPONENT_BODY(RotationComponent);
	glm::vec2 value{};

	glm::quat toQuat() const
	{
		return glm::quat{ glm::vec3{value.x, value.y, 0.f} };
	}
};

struct Scale2dComponent
{
	ECS_COMPONENT_BODY(ScaleComponent);
	glm::vec2 value{1.f};
};

using TranformComponentTypes = TypeTraits::TypeList<LocationComponent, RotationComponent, ScaleComponent>;

}

ECS_COMPONENT_VALIDATION(Ecs::LocationComponent);
ECS_COMPONENT_VALIDATION(Ecs::RotationComponent);
ECS_COMPONENT_VALIDATION(Ecs::ScaleComponent);
ECS_COMPONENT_VALIDATION(Ecs::Location2dComponent)
ECS_COMPONENT_VALIDATION(Ecs::Rotation2dComponent);
ECS_COMPONENT_VALIDATION(Ecs::Scale2dComponent);

#endif
