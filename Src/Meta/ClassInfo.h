
#ifndef META_CLASS_INFO_H
#define META_CLASS_INFO_H

#include "Meta/CtorInfo.h"
#include "Meta/PropertyInfo.h"
#include "Meta/MethodInfo.h"

namespace Meta
{

namespace Detail
{

template<uint64_t Index, typename Tuple, typename T>
void ApplyVectorTypeList(eastl::vector<T>& vector)
{
	if constexpr (Index < eastl::tuple_size_v<Tuple>)
	{
		vector.emplace_back(T{eastl::tuple_element_t<Index, Tuple>{}});
		ApplyVectorTypeList<Index + 1, Tuple>(vector);
	}
}

template<uint64_t Index, typename Tuple, typename T>
void ApplyHashMapTypeList(eastl::hash_map<id_t, T>& hash_map)
{
	if constexpr (Index < eastl::tuple_size_v<Tuple>)
	{
		using tuple_element_t = eastl::tuple_element_t<Index, Tuple>;
		hash_map.emplace(tuple_element_t::ID, T{tuple_element_t{}});
		ApplyHashMapTypeList<Index + 1, Tuple>(hash_map);
	}
}

template<size_t N>
FORCEINLINE CtorInfo* FindCompatibleCtor(const eastl::span<CtorInfo>&			 ctors,
										 const eastl::array<const TypeInfo*, N>& params_info)
{
	for (auto l_it = ctors.rbegin(); l_it != ctors.rend(); ++l_it)
	{
		const auto& l_ctor_signature_array = l_it->ParamsSignature();

		if (CompareTypeInfoArray(l_ctor_signature_array.data(), params_info.data(), l_it->NeededParamCount()))
		{
			return &*l_it;
		}
	}
	return nullptr;
}

template<typename... Args>
constexpr eastl::array<const TypeInfo*, sizeof...(Args)> CreateTypeInfoArray(Args&&... values)
{
	return {&values.Type()...};
}

} // namespace Detail

/**
 * @brief Class info class.
 *
 */
class ClassInfo
{
	friend class ClassRegistry;

public:
	enum Flags
	{
		eNone
	};

	template<typename T>
	struct Rebinder;

	template<typename T, typename CtorBindersTypeList, typename PropertyBindersTypeList, typename MethodBindersTypeList,
			 uint32_t Flags = 0>
	struct Binder
	{
		using owner_t					  = T;
		using ctor_binder_type_list_t	  = CtorBindersTypeList;
		using property_binder_type_list_t = PropertyBindersTypeList;
		using method_binder_type_list_t	  = MethodBindersTypeList;

		static constexpr auto ID	= TypeInfo::Rebinder<T>::ID;
		static constexpr auto FLAGS = 0;
	};

private:
	ClassInfo();

	template<typename T>
	EXPLICIT ClassInfo(TypeTag<T>);

public:
	ClassInfo(const ClassInfo&)				   = delete;
	ClassInfo& operator=(const ClassInfo&)	   = delete;
	ClassInfo(ClassInfo&&) NOEXCEPT			   = default;
	ClassInfo& operator=(ClassInfo&&) NOEXCEPT = default;

public:
	NODISCARD const TypeInfo& Type() const;
	NODISCARD uint32_t		  Flags() const;

public:
	NODISCARD const CtorInfo& GetCtorInfo(uint64_t index) const;

	template<size_t N>
	NODISCARD const PropertyInfo& GetPropertyInfo(const char (&name)[N]) const;
	NODISCARD const PropertyInfo& GetPropertyInfo(id_t id) const;

	template<size_t N>
	NODISCARD const MethodInfo& GetMethodInfo(const char (&name)[N]) const;
	NODISCARD const MethodInfo& GetMethodInfo(id_t id) const;

	NODISCARD bool HasCtorInfo(uint64_t index) const;

	template<size_t N>
	NODISCARD bool HasPropertyInfo(const char (&name)[N]) const;
	NODISCARD bool HasPropertyInfo(id_t id) const;

	template<size_t N>
	NODISCARD bool HasMethodInfo(const char (&name)[N]) const;
	NODISCARD bool HasMethodInfo(id_t id) const;

public:
	NODISCARD Value InvokeCtor() const;
	NODISCARD Value InvokeCtor(Value p1) const;
	NODISCARD Value InvokeCtor(Value p1, Value p2) const;
	NODISCARD Value InvokeCtor(Value p1, Value p2, Value p3) const;
	NODISCARD Value InvokeCtor(Value p1, Value p2, Value p3, Value p4) const;
	NODISCARD Value InvokeCtor(Value p1, Value p2, Value p3, Value p4, Value p5) const;
	NODISCARD Value InvokeCtor(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const;
	NODISCARD Value InvokeCtor(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7) const;
	NODISCARD Value InvokeCtor(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7, Value p8) const;

public:
	NODISCARD Value InvokeMethod(id_t id, const void* owner) const;
	NODISCARD Value InvokeMethod(id_t id, const void* owner, Value p1) const;
	NODISCARD Value InvokeMethod(id_t id, const void* owner, Value p1, Value p2) const;
	NODISCARD Value InvokeMethod(id_t id, const void* owner, Value p1, Value p2, Value p3) const;
	NODISCARD Value InvokeMethod(id_t id, const void* owner, Value p1, Value p2, Value p3, Value p4) const;
	NODISCARD Value InvokeMethod(id_t id, const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5) const;
	NODISCARD Value InvokeMethod(id_t id, const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5,
								 Value p6) const;
	NODISCARD Value InvokeMethod(id_t id, const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6,
								 Value p7) const;
	NODISCARD Value InvokeMethod(id_t id, const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6,
								 Value p7, Value p8) const;

	template<size_t N, typename... Args>
	NODISCARD Value InvokeMethod(const char (&name)[N], const void* owner, Args&&... args) const;

public:
	void			SetProperty(id_t id, void* owner, Value value) const;
	NODISCARD Value GetProperty(id_t id, const void* owner) const;

	template<size_t N>
	void SetProperty(const char (&name)[N], void* owner, Value value) const;

	template<size_t N>
	NODISCARD Value GetProperty(const char (&name)[N], const void* owner) const;

public:
	NODISCARD static const ClassInfo& None();

private:
	const TypeInfo*						type_info_;
	uint32_t							flags_;
	eastl::vector<CtorInfo>				ctors_{EASTLAllocatorType{DEBUG_NAME_VAL("Meta")}};
	eastl::hash_map<id_t, PropertyInfo> properties_{EASTLAllocatorType{DEBUG_NAME_VAL("Meta")}};
	eastl::hash_map<id_t, MethodInfo>	methods_{EASTLAllocatorType{DEBUG_NAME_VAL("Meta")}};
};

class ClassRegistry
{
public:
	ClassRegistry();

	template<typename T>
	const ClassInfo& Emplace();
	const ClassInfo& Get(const char* name);
	const ClassInfo& Get(id_t id);
	bool			 IsRegistered(const char* name) const;

	static ClassRegistry& Instance();

private:
	eastl::hash_map<id_t, ClassInfo> classes_{DEBUG_NAME_VAL("Meta")};
	static ClassRegistry*			 instance_;
};

template<typename T>
ClassInfo::ClassInfo(TypeTag<T>) : type_info_{&Typeof<typename Rebinder<T>::owner_t>()}, flags_{Rebinder<T>::FLAGS}
{
	ctors_.reserve(Rebinder<T>::ctor_binder_type_list_t::SIZE);
	properties_.reserve(Rebinder<T>::property_binder_type_list_t::SIZE);
	methods_.reserve(Rebinder<T>::method_binder_type_list_t::SIZE);

	Detail::ApplyVectorTypeList<0,
								typename TypeTraits::TlToTuple<typename Rebinder<T>::ctor_binder_type_list_t>::type_t>(
		ctors_);
	Detail::ApplyHashMapTypeList<
		0, typename TypeTraits::TlToTuple<typename Rebinder<T>::property_binder_type_list_t>::type_t>(properties_);
	Detail::ApplyHashMapTypeList<
		0, typename TypeTraits::TlToTuple<typename Rebinder<T>::method_binder_type_list_t>::type_t>(methods_);
}

template<size_t N>
const PropertyInfo& ClassInfo::GetPropertyInfo(const char (&name)[N]) const
{
	return GetPropertyInfo(Hash::Fnv1A(name));
}

template<size_t N>
const MethodInfo& ClassInfo::GetMethodInfo(const char (&name)[N]) const
{
	return GetMethodInfo(Hash::Fnv1A(name));
}

template<size_t N>
bool ClassInfo::HasPropertyInfo(const char (&name)[N]) const
{
	return HasPropertyInfo(Hash::Fnv1A(name));
}

template<size_t N>
bool ClassInfo::HasMethodInfo(const char (&name)[N]) const
{
	return HasMethodInfo(Hash::Fnv1A(name));
}

template<size_t N, typename... Args>
Value ClassInfo::InvokeMethod(const char (&name)[N], const void* owner, Args&&... args) const
{
	return InvokeMethod(Hash::Fnv1A(name), owner, eastl::forward<Args>(args)...);
}

template<size_t N>
void ClassInfo::SetProperty(const char (&name)[N], void* owner, Value value) const
{
	SetProperty(Hash::Fnv1A(name), owner, value);
}

template<size_t N>
Value ClassInfo::GetProperty(const char (&name)[N], const void* owner) const
{
	return GetProperty(Hash::Fnv1A(name), owner);
}

template<typename T>
const ClassInfo& ClassRegistry::Emplace()
{
	auto l_it = classes_.find(ClassInfo::Rebinder<T>::ID);
	if (l_it == classes_.cend())
	{
		l_it = classes_.emplace(ClassInfo::Rebinder<T>::ID, ClassInfo{TypeTag<T>{}}).first;
	}
	return l_it->second;
}

template<typename T>
const ClassInfo& Classof()
{
	return ClassRegistry::Instance().Emplace<T>();
}

template<typename T>
const ClassInfo& Classof(const TypeTag<T>)
{
	return ClassRegistry::Instance().Emplace<T>();
}

template<typename T>
const ClassInfo& Classof(const T&)
{
	return ClassRegistry::Instance().Emplace<eastl::remove_pointer_t<T>>();
}

const ClassInfo& Classof(const char* name);

const ClassInfo& Classof(id_t id);

template<typename T, T Value>
struct MemberAccessor
{
	using type_t				= T;
	static constexpr auto VALUE = Value;
};

} // namespace Meta

#endif
