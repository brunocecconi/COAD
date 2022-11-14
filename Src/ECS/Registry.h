
#ifndef ECS_REGISTRY_H
#define ECS_REGISTRY_H

#include "core/common.h"
#include "core/allocator.h"
#include "ecs/component.h"

#include <EASTL/hash_map.h>
#include <EASTL/hash_set.h>
#include <EASTL/vector.h>
#include <EASTL/queue.h>
#include <EASTL/bitvector.h>
#include <EASTL/span.h>
#include <EASTL/tuple.h>

LOG_DEFINE(ecs)

#ifndef ENTITY_ID_TYPE
#define ENTITY_ID_TYPE	u64
#endif

namespace Ecs
{

using EntityId = ENTITY_ID_TYPE;

/**
 * @brief Ecs class.
 *
*/
class Registry final
{

public:
	struct ComponentArray final
	{
		friend class EntityRegistry;

		ComponentArray(const ComponentInfo& info, u64 capacity);

	public:
		static constexpr u64 INVALID_COMPONENT_ID = U64_MAX;

		ComponentArray(ComponentArray&& other) NOEXCEPT;
		ComponentArray(const ComponentArray&) = delete;
		ComponentArray& operator=(ComponentArray&&) NOEXCEPT = default;
		ComponentArray& operator=(const ComponentArray&) = delete;
		~ComponentArray();

	public:
		template < typename Component >
		void Add(EntityId id, Component&& component);

		template < typename Component >
		void Remove(EntityId id);

		template < typename Component >
		NODISCARD Component* Get(EntityId id) const;

		NODISCARD bool Contains(EntityId id) const;

	private:
		u8* data_{}, *data_cursor_{};
		u64* eindex_{}, *eindex_end_{};
	};

	template < typename Component >
	class ComponentPtr final
	{
		friend class EntityRegistry;

		ComponentPtr(const Registry* registry, Ptr<Component> component)
			: registry_{ const_cast<Registry*>(registry) }, component_{ eastl::move(component) }
		{

		}

	public:
		using UnderlyngType = Component;

		ComponentPtr(ComponentPtr&&) NOEXCEPT = default;
		ComponentPtr(const ComponentPtr&) = default;
		ComponentPtr& operator=(ComponentPtr&&) NOEXCEPT = default;
		ComponentPtr& operator=(const ComponentPtr&) = default;
		~ComponentPtr() = default;

	public:
		auto& operator->()
		{
			return component_;
		}

		auto& operator->() const
		{
			return component_;
		}

		operator Component* ()
		{
			return component_;
		}

		operator Component* () const
		{
			return component_;
		}

		operator bool() const
		{
			return component_;
		}

	private:
		Registry* registry_;
		Ptr<Component> component_;
	};

	using SignatureType = eastl::bitset<64>;

	static constexpr EntityId INVALID_ENTITY_ID = U64_MAX;

public:
	EXPLICIT Registry(u64 capacity = 10000ull);
	Registry(Registry&& other) NOEXCEPT;
	Registry(const Registry&) = delete;
	Registry& operator=(Registry&& other) NOEXCEPT;
	Registry& operator=(const Registry&) = delete;
	~Registry();

public:
	EntityId Create();

	void Destroy(EntityId id);

public:
	template < typename ... Components, typename... Booleans >
	void SetEnabled(EntityId id, Booleans... values);

	template < typename ... Components >
	void Enable(EntityId id);

	template < typename ... Components >
	void Disable(EntityId id);

	template < typename Component >
	NODISCARD bool IsEnabled(EntityId id) const;

public:
	template < typename ... Components >
	void Add(EntityId id, Components&&... components);

	template < typename ... Components >
	void Remove(EntityId id);

	template < typename Component >
	ComponentPtr<Component> Get(EntityId id) const;

public:
	NODISCARD u64 Capacity() const;

	NODISCARD bool Contains(const EntityId* ptr) const;

	void Clear();

private:
	template < typename Component >
	void SetEnabledInternal(EntityId id, bool value);

	template < typename Component >
	void AddInternal(EntityId id, ComponentInfo* info, Component&& component);

private:
	friend class EntityViewBase;

	struct ComponentArrayElement
	{
		bool constructed;
		eastl::aligned_storage_t<sizeof(ComponentArray)> memory;
	};

	EntityId* ebegin_{}, *eend_{}, *ecursor_{};
	SignatureType* signatures_{};
	ComponentArrayElement components_map_[64] = {};
};

template <typename Component>
void Registry::ComponentArray::Add(const EntityId id, Component&& component)
{
	ENSURE_LAST_RESULT_NL();
	if (Contains(id))
	{
		RESULT_ERROR(eResultErrorEcsComponentDataAddedMoreThanOnce);
	}
	eindex_[id] = data_cursor_ - data_;
	new (data_cursor_) Component{eastl::move(component)};
	data_cursor_ += sizeof(Component);
	RESULT_OK();
}

template <typename Component>
void Registry::ComponentArray::Remove(const EntityId id)
{
	ENSURE_LAST_RESULT_NL();
	if (!Contains(id))
	{
		RESULT_ERROR(eResultErrorEcsComponentDataNotAdded);
	}
	const auto l_index_removed_entity = eindex_[id];
	reinterpret_cast<Component*>(data_ + l_index_removed_entity)->~Component();
	eindex_[id] = INVALID_COMPONENT_ID;
	RESULT_OK();
}

template <typename Component>
Component* Registry::ComponentArray::Get(const EntityId id) const
{
	if (!Contains(id))
	{
		return nullptr;
	}
	return reinterpret_cast<Component*>(data_ + eindex_[id]);
}

template < typename ... Components, typename... Booleans >
void Registry::SetEnabled(const EntityId id, Booleans... values)
{
	ENSURE_LAST_RESULT_NL();
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId);
	}
	(setEnabledInternal<Components>(id, values), ...);
	RESULT_OK();
}

template <typename ... Components>
void Registry::Enable(const EntityId id)
{
	ENSURE_LAST_RESULT_NL();
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId);
	}
	(SetEnabledInternal<Components>(id, true), ...);
	RESULT_OK();
}

template <typename ... Components>
void Registry::Disable(const EntityId id)
{
	ENSURE_LAST_RESULT_NL();
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId);
	}
	(SetEnabledInternal<Components>(id, false), ...);
	RESULT_OK();
}

template <typename Component>
bool Registry::IsEnabled(const EntityId id) const
{
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId, false);
	}
	RESULT_OK();
	return signatures_[id].test(componentInfo<Component>().Id(), false);
}

template <typename ... Components>
void Registry::Add(const EntityId id, Components&&... components)
{
	ENSURE_LAST_RESULT_NL();
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId);
	}
	(AddInternal<Components>(id,
		const_cast<ComponentInfo*>(&componentInfo<Components>()),
		eastl::forward<Components>(components)), ...);
	RESULT_OK();
}

template <typename ... Components>
void Registry::Remove(EntityId id)
{
	ENSURE_LAST_RESULT_NL();
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId);
	}
	RESULT_OK();
}

template <typename Component>
Registry::ComponentPtr<Component> Registry::Get(const EntityId id) const
{
	ENSURE_LAST_RESULT_NL(ComponentPtr<Component>{this, nullptr});
	if (!IsEnabled<Component>(id))
	{
		RESULT_ERROR(eResultErrorEcsComponentNotEnabled, ComponentPtr<Component>{this, nullptr});
	}
	RESULT_OK();
	return ComponentPtr<Component>{this,
		PTR(components_map_[componentInfo<Component>().id()].get<Component>(id))};
}

template <typename Component>
void Registry::SetEnabledInternal(const EntityId id, const bool value)
{
	ENSURE_LAST_RESULT_NL();
	const auto l_comp_info = const_cast<ComponentInfo*>(&componentInfo<Component>());
	if(signatures_[id].test(l_comp_info->Id()))
	{
		RESULT_ERROR(eResultErrorEcsComponentAlreadyEnabled);
	}
	signatures_[id].set(l_comp_info->Id(), value);
	if (value && !components_map_[l_comp_info->Id()].constructed)
	{
		components_map_[l_comp_info->Id()].constructed = true;
		new (components_map_[l_comp_info->Id()].memory.mCharData) ComponentArray{*l_comp_info, Capacity()};
	}
	RESULT_OK();
}

template <typename Component>
void Registry::AddInternal(const EntityId id, ComponentInfo* info, Component&& component)
{
	ENSURE_LAST_RESULT_NL();
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId);
	}

	// Enable component inline
	if(!signatures_[id].test(info->Id()))
	{
		signatures_[id].set(info->Id());
		if (!components_map_[info->Id()].constructed)
		{
			components_map_[info->Id()].constructed = true;
			new (components_map_[info->Id()].memory.mCharData) ComponentArray{*info, Capacity()};
		}
	}

	// Add component
	reinterpret_cast<ComponentArray*>(components_map_[info->Id()].memory.mCharData)->Add(id, eastl::forward<Component>(component));

	RESULT_OK();
}

}

#endif
