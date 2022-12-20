
#include "Meta/Value.h"

namespace Meta
{

Value::Value() : type_info_{const_cast<TypeInfo*>(&TypeInfo::None())}
{
}

Value::Value(void* memory, const TypeInfo& type_info) : type_info_{memory ? const_cast<TypeInfo*>(&type_info) : const_cast<TypeInfo*>(&TypeInfo::None())}
{
	if(memory)
	{
		if (type_info_->Size() <= sizeof iptr_)
		{
			memcpy(iptr_.mCharData, memory, type_info_->Size());
			type_info.InvokeOperation(TypeInfo::eMoveAssign, static_cast<void*>(iptr_.mCharData), memory);
		}
		else
		{
			Allocators::Default l_allocator{DEBUG_NAME_VAL("Meta")};
			if (eptr_)
			{
				l_allocator.deallocate(eptr_, type_info_->Size());
			}
			eptr_	   = l_allocator.allocate(type_info.Size());
			type_info.InvokeOperation(TypeInfo::eMoveAssign, eptr_, memory);
		}
	}
}

Value::Value(const TypeInfo& type_info) : type_info_{const_cast<TypeInfo*>(&type_info)}
{
	/*if (type_info_->Size() <= sizeof iptr_)
	{
		type_info.InvokeOperation(TypeInfo::eDefaultCtor, static_cast<void*>(iptr_.mCharData));
	}
	else
	{
		Allocators::Default l_allocator{DEBUG_NAME_VAL("Meta")};
		if (eptr_)
		{
			l_allocator.deallocate(eptr_, type_info_->Size());
		}
		eptr_	   = l_allocator.allocate(type_info.Size());
		type_info.InvokeOperation(TypeInfo::eDefaultCtor, eptr_);
	}*/
}

Value::Value(Value&& other) noexcept
	: iptr_{other.iptr_}, type_info_{other.type_info_}, eptr_{other.eptr_}
{
	other.type_info_ = nullptr;
	other.eptr_		 = nullptr;
	other.iptr_		 = {};
}

Value& Value::operator=(Value&& other) noexcept
{
	type_info_		 = other.type_info_;
	eptr_			 = other.eptr_;
	iptr_			 = other.iptr_;
	other.type_info_ = nullptr;
	other.eptr_		 = nullptr;
	other.iptr_		 = {};
	return *this;
}

Value::Value(const Value& other)
	: iptr_{other.iptr_}, type_info_{other.type_info_}, eptr_{other.eptr_}
{
	other.type_info_ = nullptr;
	other.eptr_		 = nullptr;
	other.iptr_		 = {};
}

Value& Value::operator=(const Value& other)
{
	type_info_		 = other.type_info_;
	eptr_			 = other.eptr_;
	iptr_			 = other.iptr_;
	other.type_info_ = nullptr;
	other.eptr_		 = nullptr;
	other.iptr_		 = {};
	return *this;
}

Value::~Value()
{
	if (eptr_)
	{
		type_info_->InvokeOperation(TypeInfo::eDtor, eptr_);
		Allocators::Default{DEBUG_NAME_VAL("Meta")}.deallocate(eptr_, type_info_->Size());
		eptr_ = nullptr;
	}
}

const TypeInfo& Value::Type() const
{
	return *type_info_;
}

bool Value::IsValid() const
{
	return type_info_ != nullptr;
}

Value::operator bool() const
{
	return type_info_ ? (*type_info_ == Typeof<bool>() ? AsBool() : *type_info_ != TypeInfo::None()) : false;
}

Value::operator void*() const
{
	return type_info_->Size() > sizeof iptr_ ? eptr_ : iptr_.mCharData;
}

const void* Value::AsGeneric() const
{
	return type_info_->Size() > sizeof iptr_ ? eptr_ : iptr_.mCharData;
}

const bool& Value::AsBool() const
{
	return As<bool>();
}

const int8_t& Value::AsInt8() const
{
	return As<int8_t>();
}

const int16_t& Value::AsInt16() const
{
	return As<int16_t>();
}

const int32_t& Value::AsInt32() const
{
	return As<int32_t>();
}

const int64_t& Value::AsInt64() const
{
	return As<int64_t>();
}

const uint8_t& Value::AsUint8() const
{
	return As<uint8_t>();
}

const uint16_t& Value::AsUint16() const
{
	return As<uint16_t>();
}

const uint32_t& Value::AsUint32() const
{
	return As<uint32_t>();
}

const uint64_t& Value::AsUint64() const
{
	return As<uint64_t>();
}

const float32_t& Value::AsFloat32() const
{
	return As<float32_t>();
}

const float64_t& Value::AsFloat64() const
{
	return As<float64_t>();
}

//const glm::vec2& Value::AsVec2() const
//{
//	return As<glm::vec2>();
//}
//
//const glm::vec3& Value::AsVec3() const
//{
//	return As<glm::vec3>();
//}
//
//const glm::vec4& Value::AsVec4() const
//{
//	return As<glm::vec4>();
//}
//
//const glm::quat& Value::AsQuat() const
//{
//	return As<glm::quat>();
//}
//
//const glm::mat4& Value::AsMat4() const
//{
//	return As<glm::mat4>();
//}

const eastl::string_view& Value::AsStringView() const
{
	return As<eastl::string_view>();
}

void Value::Reset()
{
	if (eptr_)
	{
		// TODO: invoke operation type info
		//eptr_dtor_(eptr_);
		Allocators::Default{DEBUG_NAME_VAL("Meta")}.deallocate(eptr_, type_info_->Size());
	}
	eptr_	   = nullptr;
	type_info_ = const_cast<TypeInfo*>(&TypeInfo::None());
	Memory::CtClearMemory<0, sizeof iptr_>(iptr_.mCharData);
}

eastl::string Value::ToString() const
{
	return type_info_->ToValueString(const_cast<void*>(AsGeneric()), 256);
}

} // namespace Meta