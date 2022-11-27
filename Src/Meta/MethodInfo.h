
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
		Uint32		args_tuple_size;
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

		template<Size Index>
		const auto& Get();

		const MethodInfo&	  info;
		Owner&				  owner;
		Uint32&				  args_tuple_size;
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
	template<Hash::fnv1a_t Id>
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
	template<Hash::fnv1a_t Id, typename Owner, typename Return, typename TypeListArgs, Uint32 OptionalParamCount = 0,
			 Uint32 Flags = eCallable>
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
	NODISCARD Hash::fnv1a_t					   Id() const;
	NODISCARD Uint32						   Flags() const;
	NODISCARD const method_params_signature_t& ParamsSignature() const;
	NODISCARD Uint32						   NeededParamCount() const;
	NODISCARD Uint32						   TotalParamCount() const;
	NODISCARD Uint32						   OptionalParamCount() const;

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
	Hash::fnv1a_t			  id_;
	Uint32					  flags_;
	method_params_signature_t params_signature_;
	binder_invoke_function_t  function_;
	Uint32					  total_param_count_;
	Uint32					  optional_param_count_;
};

template<typename Owner, typename TypeList>
MethodInfo::BodyAdapter<Owner, TypeList>::BodyAdapter(body_t& body)
	: info{*static_cast<const MethodInfo*>(body.info)}, owner{*static_cast<Owner*>(body.owner)},
	  args_tuple_size{body.args_tuple_size}, args_tuple{*static_cast<generic_tuple_args_t*>(body.args_tuple)}
{
}

template<typename Owner, typename TypeList>
template<Size Index>
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

#define META_REBINDER_METHOD(OWNER, METHOD)                                                                            \
public:                                                                                                                \
	static constexpr auto Method_##METHOD##_ID = #OWNER "::" #METHOD##_fnv1a;                                          \
                                                                                                                       \
private:                                                                                                               \
	friend struct Meta::MethodInfo::Rebinder<Method_##METHOD##_ID>

#define META_METHOD_INFO_BINDER(OWNER, NAME_SYMBOL, RETURN_TYPE, OPTIONAL_ARGS, FLAGS, ...)                            \
	template<>                                                                                                         \
	struct Meta::MethodInfo::Rebinder<OWNER::Method_##NAME_SYMBOL##_ID>                                                \
		: MethodInfo::Binder<OWNER::Method_##NAME_SYMBOL##_ID, OWNER, RETURN_TYPE, TypeTraits::TypeList<__VA_ARGS__>,  \
							 OPTIONAL_ARGS, FLAGS>

#define META_METHOD_INFO_BINDER_BODY(OWNER, NAME_SYMBOL)                                                               \
	static constexpr char NAME[64]	 = {#OWNER "::" #NAME_SYMBOL};                                                     \
	static constexpr auto MEMBER_PTR = &OWNER::NAME_SYMBOL

#define META_METHOD_INFO_BINDER_INVOKE_CUSTOM(...)                                                                     \
	static Meta::Value Invoke(body_t& body)                                                                            \
	{                                                                                                                  \
		body_adapter_t l_body{body};                                                                                   \
		if constexpr (FLAGS & Meta::MethodInfo::eCallable)                                                             \
		{                                                                                                              \
			__VA_ARGS__                                                                                                \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			return {};                                                                                                 \
		}                                                                                                              \
	}

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS0()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 0) { return (l_body.owner.*MEMBER_PTR)(); } else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS1()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 1) { return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>()); } else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS2()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 2) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>());                                       \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS3()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 3) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>());                      \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS4()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 4) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>());     \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS5()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 5) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),      \
											  l_body.Get<4>());                                                        \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS6()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 6) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),      \
											  l_body.Get<4>(), l_body.Get<5>());                                       \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS7()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 7) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),      \
											  l_body.Get<4>(), l_body.Get<5>(), l_body.Get<6>());                      \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS8()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 8) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),      \
											  l_body.Get<4>(), l_body.Get<5>(), l_body.Get<6>(), l_body.Get<7>());     \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_DEFAULT(OWNER, NAME_SYMBOL, RETURN_TYPE, OPTIONAL_ARGS, FLAGS, INVOKE_MACRO, ...)      \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_METHOD_INFO_BINDER(OWNER, NAME_SYMBOL, RETURN_TYPE, OPTIONAL_ARGS, FLAGS, __VA_ARGS__)                        \
	{                                                                                                                  \
		META_METHOD_INFO_BINDER_BODY(OWNER, NAME_SYMBOL);                                                              \
		INVOKE_MACRO();                                                                                                \
	};                                                                                                                 \
	}

#endif
