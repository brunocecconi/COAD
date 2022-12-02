
/** @file Registry.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef ECS_REGISTRY_H
#define ECS_REGISTRY_H

#include "Core/Common.h"
#include "Core/Allocator.h"
#include "ECS/Component.h"
#include "Core/Ptr.h"

#include <EASTL/hash_map.h>
#include <EASTL/hash_set.h>
#include <EASTL/vector.h>
#include <EASTL/queue.h>
#include <EASTL/bitvector.h>
#include <EASTL/span.h>
#include <EASTL/tuple.h>

LOG_DEFINE(Ecs)

#ifndef ENTITY_ID_TYPE
#define ENTITY_ID_TYPE	Uint64
#endif

#if _MSC_VER
#pragma warning (disable : 4324)
#endif

namespace Ecs
{

using entity_id_t = ENTITY_ID_TYPE;

/**
 * @brief Registry class.
 *
 * The core class used to manipulation in ecs manner.
 *
 * Features:
 * 1. Compact, used with a type list that it generates a tuple.
 * 2. Signature is based on order of type list.
 *   Ex: signature of TypeList<Component1, Component2> is the same of TypeList<Component2, Component1>,
 *	 that will be accessed by the same indices because it is a tuple that will be used. But the user can't know that,
 *	 the api is supported by use of typename.
 * 3. Contiguous in memory.
 * 4. Fixed length of entities and components memory. It cannot grow.
 *
 * Data:
 * 1. Array of entity ids.
 * 2. Array of signatures.
 * 3. Tuple of array of components.
 *
 */
template < typename TypeList >
class Registry final
{
public:
	using components_t = TypeList;
	using signature_t = eastl::bitset<components_t::SIZE>;
	static constexpr entity_id_t INVALID_ENTITY_ID = eastl::numeric_limits<Uint64>::max();

private:
	/**
	 * @brief Component array class.
	 *
	 * This class is private to use internally in @ref Registry.
	 *
	 * Data:
	 * 1. Array of components.
	 * 2. Array of entity indices.
	 * 3. Free list for removed components.
	 *
	 * Behavior:
	 * 1. @ref Add
	 * 2. @ref Remove
	 * 3. @ref Get
	 * 4. @ref Contains
	 * 5. @ref Each
	 *
	 * @tparam Component Target component type.
	 *
	*/
	template < typename Component >
	class ComponentArray final
	{
		static_assert(alignof(Component) >= sizeof(IntPtr), "Invalid min component size to be able to be wrapped by free list.");

		friend class Registry;

		struct CursorFreeList
		{
			CursorFreeList* next{};
		};

		EXPLICIT ComponentArray(Uint64 capacity);

	private:
		void Add(entity_id_t id, Component&& component, RESULT_PARAM_DEFINE);
		void Remove(entity_id_t id, RESULT_PARAM_DEFINE);
		void Each(void (*function)(Component&))
		{
			eastl::for_each(eindex_, eindex_end_, [&](Uint64& e)
			{
				if(e != INVALID_COMPONENT_ID)
				{
					function(data_[e]);
				}
			});
		}

		NODISCARD Component* Get(entity_id_t id);
		NODISCARD bool Contains(entity_id_t id) const;

	public:
		static constexpr Uint64 INVALID_COMPONENT_ID = eastl::numeric_limits<Uint64>::max();

		ComponentArray(ComponentArray&& other) NOEXCEPT = delete;
		ComponentArray(const ComponentArray&) = delete;
		ComponentArray& operator=(ComponentArray&&) NOEXCEPT = delete;
		ComponentArray& operator=(const ComponentArray&) = delete;
		~ComponentArray();

	private:
		CursorFreeList *cursor_fl_{};
		Component* data_{}, *dcursor_{};
		Uint64* eindex_{}, *eindex_end_{};
	};

	template < typename Component >
	class ComponentArrayElement
	{
	public:
		using ComponentArrayType = ComponentArray<Component>;

		bool constructed;
		ALIGNAS(64) eastl::aligned_storage_t<sizeof ComponentArrayType> memory;

	public:
		ComponentArrayElement();

	public:
		void ConstructIfAllowed(Uint64 capacity);
		void DestroyIfAllowed();
		ComponentArrayType* Get();
	};

	using component_map_tuple_t = typename TypeTraits::TlToTupleTransfer<ComponentArrayElement, components_t>::type_t;

	template < typename Component >
	static constexpr Uint64 GetComponentId();

	template < typename Component >
	ComponentArrayElement<Component>& GetComponentArrayElement();

	template < Uint64 Index >
	void ConstructComponentsMap();

	template < Uint64 Index >
	void MoveComponentsMap(component_map_tuple_t&& map);

	template < Uint64 Index >
	void DestroyComponentsMap();

	template < typename Component >
	class ComponentPtr final
	{
		friend class Registry;

		ComponentPtr(Registry* registry, Ptr<Component> component);

	public:
		using underlyng_t = Component;

		ComponentPtr(ComponentPtr&&) NOEXCEPT;
		ComponentPtr(const ComponentPtr&);
		ComponentPtr& operator=(ComponentPtr&&) NOEXCEPT;
		ComponentPtr& operator=(const ComponentPtr&);
		~ComponentPtr();

	public:
		auto& operator->();
		auto& operator->() const;
		EXPLICIT operator Component* ();
		EXPLICIT operator Component* () const;
		EXPLICIT operator bool() const;

	private:
		Registry* registry_;
		Ptr<Component> component_;
	};

public:
	EXPLICIT Registry(Uint64 capacity = 10000ull, RESULT_PARAM_DEFINE);

	Registry(Registry&& other) NOEXCEPT;
	Registry(const Registry&) = delete;
	Registry& operator=(Registry&& other) NOEXCEPT;
	Registry& operator=(const Registry&) = delete;
	~Registry();

public:
	entity_id_t Create(RESULT_PARAM_DEFINE);

	void Destroy(entity_id_t id, RESULT_PARAM_DEFINE);

public:
	template <typename ... Components>
	void Enable(entity_id_t id, RESULT_PARAM_DEFINE);

	template <typename ... Components>
	void Disable(entity_id_t id, RESULT_PARAM_DEFINE);

	template <typename Component>
	NODISCARD bool IsEnabled(entity_id_t id, RESULT_PARAM_DEFINE) const;

public:
	template <typename Component>
	void Add(entity_id_t id, Component&& component, RESULT_PARAM_DEFINE);

	template <typename Component>
	void Remove(entity_id_t id, RESULT_PARAM_DEFINE);

	template <typename Component>
	ComponentPtr<Component> Get(entity_id_t id, RESULT_PARAM_DEFINE);

	template <typename Component>
	void Each(void (*function)(Component&), RESULT_PARAM_DEFINE);

public:
	NODISCARD Uint64 Capacity(RESULT_PARAM_DEFINE) const;
	NODISCARD bool Contains(const entity_id_t* ptr, RESULT_PARAM_DEFINE) const;
	void Clear(RESULT_PARAM_DEFINE);

private:
	template <typename Component>
	void SetEnabledInternal(entity_id_t id, bool value, RESULT_PARAM_DEFINE);

private:
	entity_id_t *ebegin_{}, *eend_{}, *ecursor_{};
	signature_t *signatures_{};
	component_map_tuple_t components_map_;
};

template <typename TypeList>
template <typename Component>
Registry<TypeList>::ComponentArray<Component>::ComponentArray(const Uint64 capacity)
{
	data_ = static_cast<Component*>(EASTLAllocatorType("Ecs").allocate(capacity*sizeof(Component)));
	dcursor_ = data_;
	eindex_ = static_cast<Uint64*>(EASTLAllocatorType("Ecs").allocate(capacity*sizeof(Uint64)));
	eastl::uninitialized_fill_n(eindex_, capacity, INVALID_COMPONENT_ID);
	eindex_end_ = eindex_ + capacity;
}

template <typename TypeList>
template <typename Component>
Registry<TypeList>::ComponentArray<Component>::~ComponentArray()
{
	const auto l_size = eindex_end_ - eindex_;

	EASTLAllocatorType("Ecs").deallocate(data_, l_size*sizeof(Component));
	data_ = dcursor_ = nullptr;
	cursor_fl_ = nullptr;

	EASTLAllocatorType("Ecs").deallocate(eindex_, sizeof(Uint64));
	eindex_ = eindex_end_ = nullptr;
}

template <typename TypeList>
template <typename Component>
void Registry<TypeList>::ComponentArray<Component>::Add(const entity_id_t id, Component&& component, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	if (Contains(id))
	{
		RESULT_ERROR(eResultErrorEcsComponentDataAddedMoreThanOnce);
	}

	Component *l_target_data;
	if(!cursor_fl_)
	{
		l_target_data = reinterpret_cast<Component*>(dcursor_++);
	}
	else
	{
		l_target_data = reinterpret_cast<Component*>(cursor_fl_);
		cursor_fl_ = cursor_fl_->next;
	}

	

	eindex_[id] = l_target_data - data_;
	*l_target_data = eastl::move(component);
	RESULT_OK();
}

template <typename TypeList>
template <typename Component>
void Registry<TypeList>::ComponentArray<Component>::Remove(const entity_id_t id, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	if (!Contains(id))
	{
		RESULT_ERROR(eResultErrorEcsComponentDataNotAdded);
	}
	const auto l_index_removed_entity = eindex_[id];
	data_[l_index_removed_entity].~Component();
	eindex_[id] = INVALID_COMPONENT_ID;

	// Add to cursor free list
	auto l_new_cursor = reinterpret_cast<CursorFreeList*>(data_ + l_index_removed_entity);
	l_new_cursor->next = cursor_fl_;
	cursor_fl_ = l_new_cursor;

	RESULT_OK();
}

template <typename TypeList>
template <typename Component>
Component* Registry<TypeList>::ComponentArray<Component>::Get(const entity_id_t id)
{
	if (!Contains(id))
	{
		return nullptr;
	}
	return reinterpret_cast<Component*>(data_ + eindex_[id]);
}

template <typename TypeList>
template <typename Component>
bool Registry<TypeList>::ComponentArray<Component>::Contains(const entity_id_t id) const
{
	return eindex_[id] != INVALID_COMPONENT_ID;
}

template <typename TypeList>
template <typename Component>
Registry<TypeList>::ComponentArrayElement<Component>::ComponentArrayElement() : constructed { false }
{
	Memory::ClearMemoryType(&memory);
}

template <typename TypeList>
template <typename Component>
void Registry<TypeList>::ComponentArrayElement<Component>::ConstructIfAllowed(Uint64 capacity)
{
	if(!constructed)
	{
		constructed = true;
		new (memory.mCharData) ComponentArrayType{capacity};
	}
}

template <typename TypeList>
template <typename Component>
void Registry<TypeList>::ComponentArrayElement<Component>::DestroyIfAllowed()
{
	if(constructed)
	{
		constructed = false;
		reinterpret_cast<ComponentArrayType*>(memory.mCharData)->~ComponentArrayType();
	}
}

template <typename TypeList>
template <typename Component>
typename Registry<TypeList>::template ComponentArrayElement<Component>::ComponentArrayType* Registry<TypeList>::
ComponentArrayElement<Component>::Get()
{
	return reinterpret_cast<ComponentArrayType*>(memory.mCharData);
}

template <typename TypeList>
template <typename Component>
constexpr Uint64 Registry<TypeList>::GetComponentId()
{
	return TypeTraits::FindTupleType<ComponentArrayElement<Component>, component_map_tuple_t>();
}

template <typename TypeList>
template <typename Component>
typename Registry<TypeList>::template ComponentArrayElement<Component>& Registry<TypeList>::GetComponentArrayElement()
{
	return eastl::get<GetComponentId<Component>()>(components_map_);
}

template <typename TypeList>
template <Uint64 Index>
void Registry<TypeList>::ConstructComponentsMap()
{
	if constexpr(Index < components_t::SIZE)
	{
		using Type = eastl::tuple_element_t<Index, component_map_tuple_t>;
		new (eastl::addressof(eastl::get<Index>(components_map_))) Type{};
		ConstructComponentsMap<Index+1>();
	}
}

template <typename TypeList>
template <Uint64 Index>
void Registry<TypeList>::MoveComponentsMap(component_map_tuple_t&& map)
{
	if constexpr(Index < components_t::SIZE)
	{
		eastl::get<Index>(components_map_) = eastl::move(eastl::get<Index>(map));
		MoveComponentsMap<Index+1>();
	}
}

template <typename TypeList>
template <Uint64 Index>
void Registry<TypeList>::DestroyComponentsMap()
{
	if constexpr(Index < components_t::SIZE)
	{
		eastl::get<Index>(components_map_).DestroyIfAllowed();
		DestroyComponentsMap<Index+1>();
	}
}

template <typename TypeList>
template <typename Component>
Registry<TypeList>::ComponentPtr<Component>::ComponentPtr(Registry* registry, Ptr<Component> component)
	: registry_{ registry }, component_{ eastl::move(component) }
{

}

template <typename TypeList>
template <typename Component>
Registry<TypeList>::ComponentPtr<Component>::ComponentPtr(ComponentPtr&&) noexcept = default;
template <typename TypeList>
template <typename Component>
Registry<TypeList>::ComponentPtr<Component>::ComponentPtr(const ComponentPtr&) = default;
template <typename TypeList>
template <typename Component>
typename Registry<TypeList>::template ComponentPtr<Component>& Registry<TypeList>::ComponentPtr<Component>::operator
=(ComponentPtr&&) noexcept = default;
template <typename TypeList>
template <typename Component>
typename Registry<TypeList>::template ComponentPtr<Component>& Registry<TypeList>::ComponentPtr<Component>::operator
=(const ComponentPtr&) = default;
template <typename TypeList>
template <typename Component>
Registry<TypeList>::ComponentPtr<Component>::~ComponentPtr() = default;

template <typename TypeList>
template <typename Component>
auto& Registry<TypeList>::ComponentPtr<Component>::operator->()
{
	return component_;
}

template <typename TypeList>
template <typename Component>
auto& Registry<TypeList>::ComponentPtr<Component>::operator->() const
{
	return component_;
}

template <typename TypeList>
template <typename Component>
Registry<TypeList>::ComponentPtr<Component>::operator Component*()
{
	return component_;
}

template <typename TypeList>
template <typename Component>
Registry<TypeList>::ComponentPtr<Component>::operator Component*() const
{
	return component_;
}

template <typename TypeList>
template <typename Component>
Registry<TypeList>::ComponentPtr<Component>::operator bool() const
{
	return component_;
}

template <typename TypeList>
Registry<TypeList>::Registry(const Uint64 capacity, RESULT_PARAM_IMPL)
{
	ValidateComponentTuple<typename TypeTraits::TlToTuple<TypeList>::type_t>();

	signatures_ = static_cast<signature_t*>(EASTLAllocatorType("Ecs").allocate(capacity * sizeof(signature_t)));
	ebegin_ = static_cast<entity_id_t*>(EASTLAllocatorType("Ecs").allocate(capacity * sizeof(entity_id_t)));
	eend_ = ebegin_ + capacity;
	ecursor_ = ebegin_;
	ConstructComponentsMap<0>();
}

template <typename TypeList>
Registry<TypeList>::Registry(Registry&& other) noexcept: ebegin_{other.ebegin_}, eend_{other.eend_}, ecursor_{other.ecursor_},
                                                             signatures_{other.signatures_}
{
	MoveComponentsMap<0>(eastl::move(other.components_map_));
	other.ecursor_ = other.eend_ = other.ebegin_ = nullptr;
	other.signatures_ = nullptr;
}

template <typename TypeList>
Registry<TypeList>& Registry<TypeList>::operator=(Registry&& other) noexcept
{
	MoveComponentsMap<0>(eastl::move(other.components_map_));

	signatures_ = other.signatures_;
	ebegin_ = other.ebegin_;
	eend_ = other.eend_;
	ecursor_ = other.ecursor_;

	other.signatures_ = nullptr;
	other.ecursor_ = other.eend_ = other.ebegin_ = nullptr;
	return *this;
}

template <typename TypeList>
Registry<TypeList>::~Registry()
{
	DestroyComponentsMap<0>();

	EASTLAllocatorType("Ecs").deallocate(ebegin_, 0);
	ebegin_ = eend_ = ecursor_ = nullptr;

	EASTLAllocatorType("Ecs").deallocate(signatures_, 0);
	signatures_= nullptr;
}

template <typename TypeList>
entity_id_t Registry<TypeList>::Create(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(INVALID_ENTITY_ID);
	if (!Contains(ecursor_))
	{
		RESULT_ERROR(eResultErrorEcsNoEntityAvailable, INVALID_ENTITY_ID);
	}
	if (*ecursor_ >= Capacity())
	{
		*ecursor_ = ecursor_ - ebegin_;
	}
	const auto l_id = *ecursor_++;
	new (signatures_ + l_id) signature_t{  };
	RESULT_OK();
	return l_id;
}

template <typename TypeList>
void Registry<TypeList>::Destroy(entity_id_t id, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId);
	}
	*--ecursor_ = id;
	signatures_[id].reset();
	RESULT_OK();
}

template <typename TypeList>
template <typename ... Components>
void Registry<TypeList>::Enable(const entity_id_t id, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId);
	}
	(SetEnabledInternal<Components>(id, true), ...);
	RESULT_OK();
}

template <typename TypeList>
template <typename ... Components>
void Registry<TypeList>::Disable(const entity_id_t id, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId);
	}
	(SetEnabledInternal<Components>(id, false), ...);
	RESULT_OK();
}

template <typename TypeList>
template <typename Component>
bool Registry<TypeList>::IsEnabled(const entity_id_t id, RESULT_PARAM_IMPL) const
{
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId, false);
	}
	RESULT_OK();
	return signatures_[id].test(GetComponentId<Component>());
}

template <typename TypeList>
template <typename Component>
void Registry<TypeList>::Add(const entity_id_t id, Component&& component, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId);
	}

	constexpr Uint64 l_id = GetComponentId<Component>();
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

template <typename TypeList>
template <typename Component>
void Registry<TypeList>::Remove(const entity_id_t id, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	if (id >= Capacity())
	{
		RESULT_ERROR(eResultErrorEcsInvalidEntityId);
	}
	constexpr Uint64 l_id = GetComponentId<Component>();
	signatures_[id].set(l_id, false);
	GetComponentArrayElement<Component>().Get()->Remove(id);
	RESULT_OK();
}

template <typename TypeList>
template <typename Component>
typename Registry<TypeList>::template ComponentPtr<Component> Registry<TypeList>::Get(const entity_id_t id, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(ComponentPtr<Component>{this, nullptr});
	if (!IsEnabled<Component>(id))
	{
		RESULT_ERROR(eResultErrorEcsComponentNotEnabled, ComponentPtr<Component>{this, nullptr});
	}
	RESULT_OK();
	return ComponentPtr<Component>{this, PTR(GetComponentArrayElement<Component>().Get()->Get(id))};
}

template<typename TypeList> template<typename Component> void Registry<TypeList>::Each(void(* function)(Component&), RESULT_PARAM_IMPL)
{
	auto       l_comp       = GetComponentArrayElement<Component>().Get();
	const auto l_data       = l_comp->data_;
	auto       l_eindex     = l_comp->eindex_;
	const auto l_eindex_end = l_comp->eindex_end_;

	while(l_eindex < l_eindex_end)
	{
		if(*l_eindex != ComponentArray<Component>::INVALID_COMPONENT_ID)
		{
			function(*(l_data + *l_eindex));
		}
		++l_eindex;
	}
}

template <typename TypeList>
Uint64 Registry<TypeList>::Capacity(RESULT_PARAM_IMPL) const
{
	return eend_ - ebegin_;
}

template <typename TypeList>
bool Registry<TypeList>::Contains(const entity_id_t* ptr, RESULT_PARAM_IMPL) const
{
	return ptr >= ebegin_ && ptr < eend_;
}

template <typename TypeList>
void Registry<TypeList>::Clear(RESULT_PARAM_IMPL)
{
	//entities_.clear();

	EASTLAllocatorType("Ecs").deallocate(ebegin_, 0);
	ebegin_= eend_ = ecursor_ = nullptr;

	EASTLAllocatorType("Ecs").deallocate(signatures_, 0);
	signatures_= nullptr;

	DestroyComponentsMap<0>();
}

template <typename TypeList>
template <typename Component>
void Registry<TypeList>::SetEnabledInternal(const entity_id_t id, const bool value, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	constexpr Uint64 l_id = GetComponentId<Component>();
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

}

#endif
