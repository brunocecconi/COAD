
#ifndef ECS_REGISTRY_H
#define ECS_REGISTRY_H

#include "Core/Common.h"
#include "Core/Allocator.h"
#include "ECS/Component.h"

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

#if _MSC_VER
#pragma warning (disable : 4324)
#endif

namespace Ecs
{

using EntityId = ENTITY_ID_TYPE;

/**
 * @brief Ecs class.
 *
*/
template < typename ... TypeLists >
class Registry final
{

public:
	template < typename Component >
	struct ComponentArray final
	{
		friend class EntityRegistry;

		ComponentArray(u64 capacity)
		{
			data_ = static_cast<Component*>(EASTLAllocatorType("Ecs").allocate(capacity*sizeof(Component)));
			data_cursor_ = data_;
			eindex_ = static_cast<u64*>(EASTLAllocatorType("Ecs").allocate(capacity*sizeof(u64)));
			eastl::uninitialized_fill_n(eindex_, capacity, INVALID_COMPONENT_ID);
			eindex_end_ = eindex_ + capacity;
		}

	public:
		static constexpr u64 INVALID_COMPONENT_ID = U64_MAX;

		ComponentArray(ComponentArray&& other) NOEXCEPT
			: data_{other.data_}, data_cursor_{ other.data_cursor_ }, eindex_{ other.eindex_ }, eindex_end_{other.eindex_end_}
		{
			other.data_ = other.data_cursor_ = nullptr;
			other.eindex_ = eindex_end_ = nullptr;
		}
		ComponentArray(const ComponentArray&) = delete;
		ComponentArray& operator=(ComponentArray&&) NOEXCEPT = default;
		ComponentArray& operator=(const ComponentArray&) = delete;
		~ComponentArray()
		{
			EASTLAllocatorType("Ecs").deallocate(data_, 0);
			data_ = data_cursor_ = nullptr;

			EASTLAllocatorType("Ecs").deallocate(eindex_, 0);
			eindex_ = eindex_end_ = nullptr;
		}

	public:
		void Add(EntityId id, Component&& component)
		{
			ENSURE_LAST_RESULT_NL();
			if (Contains(id))
			{
				RESULT_ERROR(eResultErrorEcsComponentDataAddedMoreThanOnce);
			}
			eindex_[id] = data_cursor_ - data_;
			(*data_cursor_++) = eastl::move(component);
			RESULT_OK();
		}

		void Remove(EntityId id)
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

		NODISCARD Component* Get(EntityId id) const
		{
			if (!Contains(id))
			{
				return nullptr;
			}
			return reinterpret_cast<Component*>(data_ + eindex_[id]);
		}

		NODISCARD bool Contains(EntityId id) const
		{
			return eindex_[id] != INVALID_COMPONENT_ID;
		}

	private:
		Component* data_{}, *data_cursor_{};
		u64* eindex_{}, *eindex_end_{};
	};

private:
	template < typename Component >
	struct ComponentArrayElement
	{
		using ComponentArrayType = ComponentArray<Component>;

		bool constructed;
		eastl::aligned_storage_t<sizeof ComponentArrayType> memory;

		void ConstructIfAllowed(u64 capacity)
		{
			if(!constructed)
			{
				constructed = true;
				new (memory.mCharData) ComponentArrayType{capacity};
			}
		}

		void DestroyIfAllowed()
		{
			if(constructed)
			{
				constructed = false;
				reinterpret_cast<ComponentArrayType*>(memory.mCharData)->~ComponentArrayType();
			}
		}

		ComponentArrayType* Get()
		{
			return reinterpret_cast<ComponentArrayType*>(memory.mCharData);
		}
	};

	using ComponentTypes = typename TypeTraits::TlCat<TypeLists..., TypeTraits::TypeListEmpty>::Type;
	using ComponentMapTuple = typename TypeTraits::TlToTupleTransfer<ComponentArrayElement, ComponentTypes>::Type;

	template < typename Component >
	static constexpr u64 GetComponentId()
	{
		return TypeTraits::FindTupleType<ComponentArrayElement<Component>, ComponentMapTuple>();
	}

	template < typename Component >
	ComponentArrayElement<Component>& GetComponentArrayElement()
	{
		return eastl::get<GetComponentId<Component>()>(components_map_);
	}

	template < u64 Index >
	void ConstructComponentsMap()
	{
		if constexpr(Index < ComponentTypes::SIZE)
		{
			new (eastl::get<Index>(components_map_).memory.mCharData) eastl::tuple_element_t<Index, ComponentMapTuple>{};
			DestroyComponentsMap<Index+1>();
		}
	}

	template < u64 Index >
	void DestroyComponentsMap()
	{
		if constexpr(Index < ComponentTypes::SIZE)
		{
			eastl::get<Index>(components_map_).DestroyIfAllowed();
			DestroyComponentsMap<Index+1>();
		}
	}

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

	using SignatureType = eastl::bitset<ComponentTypes::SIZE>;

	static constexpr EntityId INVALID_ENTITY_ID = U64_MAX;

public:
	EXPLICIT Registry(u64 capacity = 10000ull)
	{
		signatures_ = static_cast<SignatureType*>(EASTLAllocatorType("Ecs").allocate(capacity * sizeof(SignatureType)));
		ebegin_ = static_cast<EntityId*>(EASTLAllocatorType("Ecs").allocate(capacity * sizeof(EntityId)));
		eend_ = ebegin_ + capacity;
		ecursor_ = ebegin_;
		ConstructComponentsMap<0>();
		//eastl::uninitialized_default_fill_n(components_map_, 64);
	}
	Registry(Registry&& other) NOEXCEPT
		: ebegin_{other.ebegin_}, eend_{other.eend_}, ecursor_{other.ecursor_},
		signatures_{other.signatures_}
		{
			memcpy(components_map_, other.components_map_, sizeof components_map_);
		other.ecursor_ = other.eend_ = other.ebegin_ = nullptr;
		other.signatures_ = nullptr;
	}
	Registry(const Registry&) = delete;
	Registry& operator=(Registry&& other) NOEXCEPT
	{
		memcpy(components_map_, other.components_map_, sizeof components_map_);

		signatures_ = other.signatures_;
		ebegin_ = other.ebegin_;
		eend_ = other.eend_;
		ecursor_ = other.ecursor_;

		other.signatures_ = nullptr;
		other.ecursor_ = other.eend_ = other.ebegin_ = nullptr;
		return *this;
	}
	Registry& operator=(const Registry&) = delete;
	~Registry()
	{
		DestroyComponentsMap<0>();

		EASTLAllocatorType("Ecs").deallocate(ebegin_, 0);
		ebegin_ = eend_ = ecursor_ = nullptr;

		EASTLAllocatorType("Ecs").deallocate(signatures_, 0);
		signatures_= nullptr;
	}

public:
	EntityId Create()
	{
		ENSURE_LAST_RESULT_NL(INVALID_ENTITY_ID);
		if (!Contains(ecursor_))
		{
			RESULT_ERROR(eResultErrorEcsNoEntityAvailable, INVALID_ENTITY_ID);
		}
		RESULT_OK();
		if (!(*ecursor_ >= 0ull && *ecursor_ < Capacity()))
		{
			*ecursor_ = ecursor_ - ebegin_;
		}
		const auto l_id = *ecursor_++;
		new (signatures_ + l_id) SignatureType{  };
		return l_id;
	}

	void Destroy(EntityId id)
	{
		ENSURE_LAST_RESULT_NL();
		if (id >= Capacity())
		{
			RESULT_ERROR(eResultErrorEcsInvalidEntityId);
		}
		*--ecursor_ = id;
		signatures_[id].reset();
		RESULT_OK();
	}

public:
	template < typename ... Components, typename... Booleans >
	void SetEnabled(const EntityId id, Booleans... values)
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
	void Enable(const EntityId id)
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
	void Disable(const EntityId id)
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
	bool IsEnabled(const EntityId id) const
	{
		if (id >= Capacity())
		{
			RESULT_ERROR(eResultErrorEcsInvalidEntityId, false);
		}
		RESULT_OK();
		return signatures_[id].test(GetComponentId<Component>(), false);
	}

public:
	template <typename ... Components>
	void Add(const EntityId id, Components&&... components)
	{
		ENSURE_LAST_RESULT_NL();
		if (id >= Capacity())
		{
			RESULT_ERROR(eResultErrorEcsInvalidEntityId);
		}
		(AddInternal<Components>(id, eastl::forward<Components>(components)), ...);
		RESULT_OK();
	}

	template <typename ... Components>
	void Remove(EntityId id)
	{
		ENSURE_LAST_RESULT_NL();
		if (id >= Capacity())
		{
			RESULT_ERROR(eResultErrorEcsInvalidEntityId);
		}
		RESULT_OK();
	}

	template <typename Component>
	ComponentPtr<Component> Get(const EntityId id) const
	{
		ENSURE_LAST_RESULT_NL(ComponentPtr<Component>{this, nullptr});
		if (!IsEnabled<Component>(id))
		{
			RESULT_ERROR(eResultErrorEcsComponentNotEnabled, ComponentPtr<Component>{this, nullptr});
		}
		RESULT_OK();
		return ComponentPtr<Component>{this, PTR(GetComponentArrayElement<Component>().Get()->Get<Component>(id))};
	}

public:
	NODISCARD u64 Capacity() const
	{
		return eend_ - ebegin_;
	}

	NODISCARD bool Contains(const EntityId* ptr) const
	{
		return ptr >= ebegin_ && ptr < eend_;
	}

	void Clear()
	{
		//entities_.clear();

		EASTLAllocatorType("Ecs").deallocate(ebegin_, 0);
		ebegin_= eend_ = ecursor_ = nullptr;

		EASTLAllocatorType("Ecs").deallocate(signatures_, 0);
		signatures_= nullptr;

		DestroyComponentsMap<0>();
	}

private:
	template <typename Component>
	void SetEnabledInternal(const EntityId id, const bool value)
	{
		ENSURE_LAST_RESULT_NL();
		constexpr u64 l_id = GetComponentId<Component>();
		if(signatures_[id].test(l_id))
		{
			RESULT_ERROR(eResultErrorEcsComponentAlreadyEnabled);
		}
		signatures_[id].set(l_id, value);
		if(value)
		{
			GetComponentArrayElement<Component>().ConstructIfAllowed(Capacity());
		}
		RESULT_OK();
	}

	template <typename Component>
	void AddInternal(const EntityId id, Component&& component)
	{
		ENSURE_LAST_RESULT_NL();
		if (id >= Capacity())
		{
			RESULT_ERROR(eResultErrorEcsInvalidEntityId);
		}

		constexpr u64 l_id = GetComponentId<Component>();
		auto& l_component_element = GetComponentArrayElement<Component>();

		// Enable component inline
		if(!signatures_[id].test(l_id))
		{
			signatures_[id].set(l_id);
			l_component_element.ConstructIfAllowed(Capacity());
		}

		// Add component
		l_component_element.Get()->Add(id, eastl::forward<Component>(component));

		RESULT_OK();
	}

private:
	EntityId* ebegin_{}, *eend_{}, *ecursor_{};
	SignatureType* signatures_{};
	ComponentMapTuple components_map_;
};

}

#endif
