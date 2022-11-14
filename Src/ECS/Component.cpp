
#include "ecs/component.h"

static ComponentInfo::IdType g_component_info_unique_id{};

ComponentInfo::ComponentInfo(const Meta::TypeInfo& type_info)
	: id_{g_component_info_unique_id++}, type_info_{&type_info}
{
}

ComponentInfo::ComponentInfo(const ComponentInfo&) = default;
ComponentInfo::ComponentInfo(ComponentInfo&&) noexcept = default;
ComponentInfo& ComponentInfo::operator=(const ComponentInfo&) = default;
ComponentInfo& ComponentInfo::operator=(ComponentInfo&&) noexcept = default;
ComponentInfo::~ComponentInfo() = default;

ComponentInfo::IdType ComponentInfo::Id() const
{
	return id_;
}

const Meta::TypeInfo& ComponentInfo::TypeInfo() const
{
	return *type_info_;
}

ComponentInfo::IdType ComponentInfo::CurrentUniqueId()
{
	return g_component_info_unique_id;
}
