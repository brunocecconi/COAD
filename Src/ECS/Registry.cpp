
#include "ecs/registry.h"

namespace Ecs
{

Registry::ComponentArray::ComponentArray(const ComponentInfo& info, const u64 capacity)
{
	data_ = static_cast<u8*>(EASTLAllocatorType("Ecs").allocate(capacity*info.TypeInfo().size()));
	data_cursor_ = data_;
	eindex_ = static_cast<u64*>(EASTLAllocatorType("Ecs").allocate(capacity*sizeof(u64)));
	eastl::uninitialized_fill_n(eindex_, capacity, INVALID_COMPONENT_ID);
	eindex_end_ = eindex_ + capacity;
}

Registry::ComponentArray::ComponentArray(ComponentArray&& other) noexcept
	: data_{other.data_}, data_cursor_{ other.data_cursor_ }, eindex_{ other.eindex_ }, eindex_end_{other.eindex_end_}
{
	other.data_ = other.data_cursor_ = nullptr;
	other.eindex_ = eindex_end_ = nullptr;
}

Registry::ComponentArray::~ComponentArray()
{
	EASTLAllocatorType("Ecs").deallocate(data_, 0);
	data_ = data_cursor_ = nullptr;

	EASTLAllocatorType("Ecs").deallocate(eindex_, 0);
	eindex_ = eindex_end_ = nullptr;
}

bool Registry::ComponentArray::Contains(const EntityId id) const
{
	return eindex_[id] != INVALID_COMPONENT_ID;
}

Registry::Registry(const u64 capacity)
{
	signatures_ = static_cast<SignatureType*>(EASTLAllocatorType("Ecs").allocate(capacity * sizeof(SignatureType)));
	ebegin_ = static_cast<EntityId*>(EASTLAllocatorType("Ecs").allocate(capacity * sizeof(EntityId)));
	eend_ = ebegin_ + capacity;
	ecursor_ = ebegin_;
	eastl::uninitialized_default_fill_n(components_map_, 64);
}

Registry::Registry(Registry&& other) noexcept
	: ebegin_{other.ebegin_}, eend_{other.eend_}, ecursor_{other.ecursor_},
	signatures_{other.signatures_}
{
	memcpy(components_map_, other.components_map_, sizeof components_map_);
	other.ecursor_ = other.eend_ = other.ebegin_ = nullptr;
	other.signatures_ = nullptr;
}

Registry& Registry::operator=(Registry&& other) noexcept
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

Registry::~Registry()
{
	for (auto& [constructed, memory] : components_map_)
	{
		if(constructed)
		{
			reinterpret_cast<ComponentArray*>(memory.mCharData)->~ComponentArray();
		}
	}

	EASTLAllocatorType("Ecs").deallocate(ebegin_, 0);
	ebegin_ = eend_ = ecursor_ = nullptr;

	EASTLAllocatorType("Ecs").deallocate(signatures_, 0);
	signatures_= nullptr;
}

EntityId Registry::Create()
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

void Registry::Destroy(const EntityId id)
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

u64 Registry::Capacity() const
{
	return eend_ - ebegin_;
}

bool Registry::Contains(const EntityId* ptr) const
{
	return ptr >= ebegin_ && ptr < eend_;
}

void Registry::Clear()
{
	//entities_.clear();

	EASTLAllocatorType("Ecs").deallocate(ebegin_, 0);
	ebegin_= eend_ = ecursor_ = nullptr;

	EASTLAllocatorType("Ecs").deallocate(signatures_, 0);
	signatures_= nullptr;

	for (auto& [constructed, memory] : components_map_)
	{
		if(constructed)
		{
			reinterpret_cast<ComponentArray*>(memory.mCharData)->~ComponentArray();
		}
	}
}

}
