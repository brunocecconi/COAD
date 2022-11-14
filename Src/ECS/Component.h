
#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include "core/common.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define ECS_COMPONENT_BODY(NAME)	\
public:	\
CLASS_BODY_ONLY_HEADER(NAME)	\
struct ecs_component_type{};

#define ECS_COMPONENT_VALIDATION(NAME)	\
TYPE_INFO_DEFINE(NAME);	\
static_assert(!eastl::is_polymorphic_v<NAME>, \
"The component class cannot be polymorphic.");	\
static_assert(eastl::is_same_v<eastl::void_t<typename NAME::ecs_component_type>, void>, \
"Target type is not an entity component.");	\
template<> FORCEINLINE const ComponentInfo& componentInfo<NAME>()	\
{	\
	static ComponentInfo l_value{Meta::type<NAME>()};	\
	return l_value;	\
}

#ifndef COMPONENT_ID_TYPE
#define COMPONENT_ID_TYPE	u64
#endif

struct ComponentInfo 
{
	using IdType = COMPONENT_ID_TYPE;

public:
	EXPLICIT ComponentInfo(const Meta::TypeInfo& type_info);
	ComponentInfo(const ComponentInfo&);
	ComponentInfo(ComponentInfo&&) NOEXCEPT;
	ComponentInfo& operator=(const ComponentInfo&);
	ComponentInfo& operator=(ComponentInfo&&) NOEXCEPT;
	~ComponentInfo();

public:
	NODISCARD IdType Id() const;
	NODISCARD const Meta::TypeInfo& TypeInfo() const;
	NODISCARD static IdType CurrentUniqueId();

public:
	template < typename Allocator = EASTLAllocatorType >
	NODISCARD eastl::basic_string<char, Allocator> ToString(
		const Allocator& allocator = NAME_VAL("ComponentInfo")) const;

private:
	IdType id_;
	const Meta::TypeInfo* type_info_;
};

template <typename Allocator>
eastl::basic_string<char, Allocator> ComponentInfo::ToString(const Allocator& allocator) const
{
	char l_str[1024];
	sprintf(l_str, "ComponentInfo(id=%llu,type_info=%s)", id_, type_info_->ToString().c_str());
	return eastl::basic_string<char, Allocator>{ l_str, allocator };
}

//namespace ecs
//{

template < typename Component >
const ComponentInfo& componentInfo();

namespace component
{
enum Type
{
	eActive,
	eName,
	eLocation,
	eRotation,
	eScale
};
}

//template < component::Type Component >
//struct Component;

struct LocationComponent
{
	ECS_COMPONENT_BODY(LocationComponent)
	glm::vec3 value{};
};
ECS_COMPONENT_VALIDATION(LocationComponent);

struct RotationComponent
{
	ECS_COMPONENT_BODY(RotationComponent);
	glm::vec3 value{};

	glm::quat toQuat() const
	{
		return glm::quat{ value };
	}
};
ECS_COMPONENT_VALIDATION(RotationComponent);

struct ScaleComponent
{
	ECS_COMPONENT_BODY(ScaleComponent);
	glm::vec3 value{1.f};
};
ECS_COMPONENT_VALIDATION(ScaleComponent);

struct Location2dComponent
{
	ECS_COMPONENT_BODY(LocationComponent);
	glm::vec2 value{};
};
ECS_COMPONENT_VALIDATION(Location2dComponent)

struct Rotation2dComponent
{
	ECS_COMPONENT_BODY(RotationComponent);
	glm::vec2 value{};

	glm::quat toQuat() const
	{
		return glm::quat{ glm::vec3{value.x, value.y, 0.f} };
	}
};
ECS_COMPONENT_VALIDATION(Rotation2dComponent);

struct Scale2dComponent
{
	ECS_COMPONENT_BODY(ScaleComponent);
	glm::vec2 value{1.f};
};
ECS_COMPONENT_VALIDATION(Scale2dComponent);

//}

#endif
