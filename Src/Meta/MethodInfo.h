
#ifndef META_METHOD_INFO_H
#define META_METHOD_INFO_H

#include "Meta/TypeInfo.h"
#include "Meta/CtorInfo.h"

namespace Meta
{

class MethodInfo
{
public:
	using method_params_signature_t = eastl::array<const TypeInfo*, 8>;

	enum Flags
	{
		eNone,
		eCallable,
		eStatic
	};

	struct Body
	{
		const void* info;
		void*		owner;
		Uint32		args_tuple_size;
		void*		args_tuple;
	};

	template<typename Owner, typename TypeList>
	struct BodyAdapter
	{
		using owner_t		   = Owner;
		using body_t		   = Body;
		using arguments_type_t = typename TypeTraits::TlToTuple<TypeList>::type_t;

		EXPLICIT BodyAdapter(body_t& body);

		const MethodInfo& info;
		Owner&			  owner;
		Uint32&			  args_tuple_size;
		arguments_type_t& args_tuple;
	};

	template<typename Owner, typename Return, typename TypeListArgs, Uint32 OptionalParamCount = 0,
			 Uint32 Flags = eCallable>
	struct Binder
	{
		using owner_t	   = Owner;
		using return_t	   = Return;
		using args_t	   = TypeListArgs;
		using tuple_args_t = typename TypeTraits::TlToTuple<args_t>::type_t;
		using body_t	   = Body;

		static constexpr auto	FLAGS				 = Flags;
		static constexpr Uint32 OPTIONAL_PARAM_COUNT = OptionalParamCount;

		template<Size N>
		EXPLICIT constexpr Binder(const char (&name)[N]);

		NODISCARD constexpr const char* Name() const;

	private:
		const char* name_;
	};
	using binder_invoke_function_t = Value (*)(Body&);

	template<typename BinderType>
	EXPLICIT MethodInfo(BinderType);

public:
	NODISCARD const TypeInfo&				   OwnerType() const;
	NODISCARD const TypeInfo&				   ReturnType() const;
	NODISCARD const char*					   Name() const;
	NODISCARD Uint32						   Flags() const;
	NODISCARD const method_params_signature_t& ParamsSignature() const;
	NODISCARD Uint32						   NeededParamCount() const;
	NODISCARD Uint32						   TotalParamCount() const;
	NODISCARD Uint32						   OptionalParamCount() const;

public:
	Value Invoke(const void* owner) const;
	Value Invoke(const void* owner, Value p1) const;
	Value Invoke(const void* owner, Value p1, Value p2) const;
	Value Invoke(const void* owner, Value p1, Value p2, Value p3) const;
	Value Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4) const;
	Value Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5) const;
	Value Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const;
	Value Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7) const;
	Value Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7, Value p8) const;

private:
	const TypeInfo&			  owner_type_;
	const TypeInfo&			  return_type_;
	const char*				  name_;
	Hash::fnv1a_t			  id_;
	Uint32					  flags_;
	method_params_signature_t params_signature_;
	binder_invoke_function_t  function_;
	Uint32					  total_param_count_;
	Uint32					  optional_param_count_;
};

template<typename Owner, typename Return, typename TypeListArgs, Uint32 OptionalParamCount, Uint32 Flags>
template<Size N>
constexpr MethodInfo::Binder<Owner, Return, TypeListArgs, OptionalParamCount, Flags>::Binder(const char (&name)[N])
	: name_{name}
{
}

template<typename Owner, typename Return, typename TypeListArgs, Uint32 OptionalParamCount, Uint32 Flags>
constexpr const char* MethodInfo::Binder<Owner, Return, TypeListArgs, OptionalParamCount, Flags>::Name() const
{
	return name_;
}

template<typename Owner, typename TypeList>
MethodInfo::BodyAdapter<Owner, TypeList>::BodyAdapter(body_t& body)
	: info{*static_cast<const MethodInfo*>(body.info)}, owner{*static_cast<Owner*>(body.owner)},
	  args_tuple_size{body.args_tuple_size}, args_tuple{*static_cast<arguments_type_t*>(body.args_tuple)}
{
}

template<typename BinderType>
MethodInfo::MethodInfo(BinderType binder)
	: owner_type_{Typeof<typename BinderType::owner_t>()}, return_type_{Typeof<typename BinderType::return_t>()},
	  name_{binder.Name()}, id_{BinderType::ID}, flags_{BinderType::FLAGS}, function_{&BinderType::Invoke},
	  total_param_count_{BinderType::args_t::SIZE}, optional_param_count_{BinderType::OPTIONAL_PARAM_COUNT}
{
	static_assert(BinderType::args_t::SIZE < 8, "Parameters signature count above allowed. Max is 10.");
	Detail::ApplyParamsSignature<0, BinderType::args_t::SIZE, typename BinderType::tuple_args_t>(params_signature_);
}

} // namespace Meta

#endif
