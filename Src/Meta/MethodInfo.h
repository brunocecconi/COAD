
#ifndef META_METHOD_INFO_H
#define META_METHOD_INFO_H

#include "Meta/TypeInfo.h"
#include "Meta/CtorInfo.h"

namespace Meta
{

/**
 * @brief Method info class.
 * 
*/
class MethodInfo
{
public:
	using method_params_signature_t = eastl::array<const TypeInfo*, 8>;

	/**
	 * @brief Flags.
	 * 
	*/
	enum Flags
	{
		eNone,
		eCallable
	};

	/**
	 * @brief Body structure.
	 * 
	*/
	struct Body
	{
		const void* info;
		void*		owner;
		uint32_t		args_tuple_size;
		void*		args_tuple;
	};

	/**
	 * @brief Body adapter class.
	 *
	 * Used inside a reflected function to a method.
	 *
	 * @tparam TypeList Target type list to the arguments.
	 *
	 */
	template<typename Owner, typename TypeList>
	struct BodyAdapter
	{
		using owner_t				   = Owner;
		using body_t				   = Body;
		using generic_type_list_args_t = typename TypeTraits::TlCreateTypeList<Value, TypeList::SIZE>::type_t;
		using tuple_args_t			   = typename TypeTraits::TlToTuple<TypeList>::type_t;
		using generic_tuple_args_t	   = typename TypeTraits::TlToTuple<generic_type_list_args_t>::type_t;

		EXPLICIT BodyAdapter(body_t& body);

		template<size_t Index>
		const auto& Get();

		const MethodInfo&	  info;
		Owner&				  owner;
		uint32_t&				  args_tuple_size;
		generic_tuple_args_t& args_tuple;
	};

	/**
	 * @brief Rebinder structure.
	 * 
	 * Used to provide information.
	 * 
	 * @tparam Id Hash name id.
	 * 
	*/
	template<id_t Id>
	struct Rebinder;

	/**
	 * @brief Binder structure.
	 * 
	 * Used to provide information.
	 * 
	 * @tparam Owner Owner type.
	 * @tparam Return Return type.
	 * @tparam TypeListArgs Type list to argument types.
	 * @tparam OptionalParamCount Amount of optional parameters.
	 * @tparam Flags Flags value.
	 * @tparam Id Hash name id.
	 * 
	*/
	template<id_t Id, typename Owner, typename Return, typename TypeListArgs, uint32_t OptionalParamCount = 0,
			 uint32_t Flags = eCallable>
	struct Binder
	{
		using owner_t		 = Owner;
		using return_t		 = Return;
		using args_t		 = TypeListArgs;
		using tuple_args_t	 = typename TypeTraits::TlToTuple<args_t>::type_t;
		using body_t		 = Body;
		using body_adapter_t = BodyAdapter<owner_t, args_t>;

		static constexpr auto ID				   = Id;
		static constexpr auto FLAGS				   = Flags;
		static constexpr auto OPTIONAL_PARAM_COUNT = OptionalParamCount;
	};
	using binder_invoke_function_t = Value (*)(Body&);

	template<typename BinderType>
	EXPLICIT MethodInfo(BinderType);

public:
	NODISCARD const TypeInfo& OwnerType() const;
	NODISCARD const TypeInfo& ReturnType() const;
	NODISCARD const char*	  Name() const;
	NODISCARD id_t					   Id() const;
	NODISCARD uint32_t						   Flags() const;
	NODISCARD const method_params_signature_t& ParamsSignature() const;
	NODISCARD uint32_t						   NeededParamCount() const;
	NODISCARD uint32_t						   TotalParamCount() const;
	NODISCARD uint32_t						   OptionalParamCount() const;

public:
	NODISCARD Value Invoke(const void* owner) const;
	NODISCARD Value Invoke(const void* owner, Value p1) const;
	NODISCARD Value Invoke(const void* owner, Value p1, Value p2) const;
	NODISCARD Value Invoke(const void* owner, Value p1, Value p2, Value p3) const;
	NODISCARD Value Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4) const;
	NODISCARD Value Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5) const;
	NODISCARD Value Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const;
	NODISCARD Value Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6,
						   Value p7) const;
	NODISCARD Value Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7,
						   Value p8) const;

	NODISCARD Value operator()(const void* owner) const;
	NODISCARD Value operator()(const void* owner, Value p1) const;
	NODISCARD Value operator()(const void* owner, Value p1, Value p2) const;
	NODISCARD Value operator()(const void* owner, Value p1, Value p2, Value p3) const;
	NODISCARD Value operator()(const void* owner, Value p1, Value p2, Value p3, Value p4) const;
	NODISCARD Value operator()(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5) const;
	NODISCARD Value operator()(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const;
	NODISCARD Value operator()(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6,
							   Value p7) const;
	NODISCARD Value operator()(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7,
							   Value p8) const;

private:
	const TypeInfo&			  owner_type_;
	const TypeInfo&			  return_type_;
	const char*				  name_;
	id_t			  id_;
	uint32_t					  flags_;
	method_params_signature_t params_signature_;
	binder_invoke_function_t  function_;
	uint32_t					  total_param_count_;
	uint32_t					  optional_param_count_;
};

template<typename Owner, typename TypeList>
MethodInfo::BodyAdapter<Owner, TypeList>::BodyAdapter(body_t& body)
	: info{*static_cast<const MethodInfo*>(body.info)}, owner{*static_cast<Owner*>(body.owner)},
	  args_tuple_size{body.args_tuple_size}, args_tuple{*static_cast<generic_tuple_args_t*>(body.args_tuple)}
{
}

template<typename Owner, typename TypeList>
template<size_t Index>
const auto& MethodInfo::BodyAdapter<Owner, TypeList>::Get()
{
	return eastl::get<Index>(args_tuple).template As<eastl::tuple_element_t<Index, tuple_args_t>>();
}

template<typename BinderType>
MethodInfo::MethodInfo(BinderType)
	: owner_type_{Typeof<typename BinderType::owner_t>()}, return_type_{Typeof<typename BinderType::return_t>()},
	  name_{BinderType::NAME}, id_{BinderType::ID}, flags_{BinderType::FLAGS}, function_{&BinderType::Invoke},
	  total_param_count_{BinderType::args_t::SIZE}, optional_param_count_{BinderType::OPTIONAL_PARAM_COUNT}
{
	static_assert(BinderType::args_t::SIZE < 8, "Parameters signature count above allowed. Max is 10.");
	Detail::ApplyParamsSignature<0, BinderType::args_t::SIZE, typename BinderType::tuple_args_t>(params_signature_);
}

} // namespace Meta

#endif
