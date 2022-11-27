
#ifndef META_CTOR_INFO_H
#define META_CTOR_INFO_H

#include "Core/Any.h"
#include "Core/Assert.h"
#include "Meta/TypeInfo.h"
#include "Meta/Value.h"

namespace Meta
{

namespace Detail
{

template<Uint32 Index, Uint32 Max, typename Tuple, typename Bitset>
constexpr void ApplyParamsSignature(Bitset& signature)
{
	static_assert(Bitset::count >= Max, "Bitset size is lower than specified max value.");

	if constexpr (Index < Max)
	{
		signature[Index] = &Typeof<eastl::tuple_element_t<Index, Tuple>>();
		ApplyParamsSignature<Index + 1, Max, Tuple>(signature);
	}
}

template<Uint32 Index, typename Tuple, Uint32 ArraySize>
constexpr void GetParamsSignatureFromTuple(eastl::array<const TypeInfo*, ArraySize>& arr, Tuple& tuple)
{
	if constexpr (Index < eastl::tuple_size_v<Tuple>)
	{
		arr[Index] = &eastl::get<Index>(tuple).Type();
		GetParamsSignatureFromTuple<Index + 1>(arr, tuple);
	}
}

template<typename Array, typename Tuple>
constexpr Array GetParamsSignatureFromTuple(Tuple& tuple)
{
	Array l_array{};
	Detail::GetParamsSignatureFromTuple<0, Tuple>(l_array, tuple);
	return l_array;
}

} // namespace Detail

/**
 * @brief Ctor info class.
 *
 */
class CtorInfo
{
public:
	using method_params_signature_t = eastl::array<const TypeInfo*, 8>;

	/**
	 * @brief Body structure.
	 *
	 * Used to pass information from the invoker function.
	 *
	 */
	struct Body
	{
		const void* info;
		void*		memory;
		Uint32		args_tuple_size;
		void*		args_tuple;
	};

	/**
	 * @brief Body adapter class.
	 *
	 * Used inside a reflected function to a ctor.
	 *
	 * @tparam TypeList Target type list to the arguments.
	 *
	 */
	template<typename TypeList>
	struct BodyAdapter
	{
		using body_t				   = Body;
		using generic_type_list_args_t = typename TypeTraits::TlCreateTypeList<Value, TypeList::SIZE>::type_t;
		using tuple_args_t			   = typename TypeTraits::TlToTuple<TypeList>::type_t;
		using generic_tuple_args_t	   = typename TypeTraits::TlToTuple<generic_type_list_args_t>::type_t;

		EXPLICIT BodyAdapter(body_t& structure);

		template<Size Index>
		const auto& Get();

		const CtorInfo&		  info;
		void*				  memory;
		Uint32&				  args_tuple_count;
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
	 * @brief Ctor binder.
	 *
	 * Used to apply information about a ctor and to implement the @ref Invoke function that
	 * will be properly call type constructor.
	 *
	 * @tparam CtorFunction Ctor function type.
	 * @tparam OptionalParamCount Number of optional arguments. Later used for heuristics.
	 *
	 */
	template<Hash::fnv1a_t Id, typename CtorFunction, Uint32 OptionalParamCount>
	struct Binder
	{
		using owner_t		 = typename TypeTraits::FunctionTraits<CtorFunction>::return_t;
		using args_t		 = typename TypeTraits::FunctionTraits<CtorFunction>::param_type_list_t;
		using body_t		 = Body;
		using body_adapter_t = BodyAdapter<args_t>;

		static constexpr auto ID = Id;
		static constexpr auto OPTIONAL_PARAM_COUNT = OptionalParamCount;
	};
	using binder_invoke_function_t = void (*)(Body&);

	/**
	 * @brief Ctor
	 * 
	 * @tparam RebinderType Type the implemented Rebinder.
	 * @param Anonymous param.
	 * 
	*/
	template<typename RebinderType>
	EXPLICIT CtorInfo(RebinderType);

public:
	NODISCARD const TypeInfo&				   OwnerType() const;
	NODISCARD const method_params_signature_t& ParamsSignature() const;
	NODISCARD Uint32						   NeededParamCount() const;
	NODISCARD Hash::fnv1a_t Id() const;
	NODISCARD Uint32		TotalParamCount() const;
	NODISCARD Uint32		OptionalParamCount() const;

public:
	NODISCARD Value Invoke() const;
	NODISCARD Value Invoke(Value p1) const;
	NODISCARD Value Invoke(Value p1, Value p2) const;
	NODISCARD Value Invoke(Value p1, Value p2, Value p3) const;
	NODISCARD Value Invoke(Value p1, Value p2, Value p3, Value p4) const;
	NODISCARD Value Invoke(Value p1, Value p2, Value p3, Value p4, Value p5) const;
	NODISCARD Value Invoke(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const;
	NODISCARD Value Invoke(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7) const;
	NODISCARD Value Invoke(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7, Value p8) const;

	NODISCARD Value operator()() const;
	NODISCARD Value operator()(Value p1) const;
	NODISCARD Value operator()(Value p1, Value p2) const;
	NODISCARD Value operator()(Value p1, Value p2, Value p3) const;
	NODISCARD Value operator()(Value p1, Value p2, Value p3, Value p4) const;
	NODISCARD Value operator()(Value p1, Value p2, Value p3, Value p4, Value p5) const;
	NODISCARD Value operator()(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const;
	NODISCARD Value operator()(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7) const;
	NODISCARD Value operator()(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7, Value p8) const;

private:
	const TypeInfo&			  owner_type_;
	method_params_signature_t params_signature_{};
	binder_invoke_function_t  function_;
	Hash::fnv1a_t			  id_;
	Uint32					  total_param_count_;
	Uint32					  optional_param_count_;
};

template<typename TypeList>
CtorInfo::BodyAdapter<TypeList>::BodyAdapter(body_t& structure)
	: info{*static_cast<const CtorInfo*>(structure.info)}, memory{structure.memory},
	  args_tuple_count{structure.args_tuple_size}, args_tuple{*static_cast<generic_tuple_args_t*>(structure.args_tuple)}
{
}

template<typename TypeList>
template<Size Index>
const auto& CtorInfo::BodyAdapter<TypeList>::Get()
{
	return eastl::get<Index>(args_tuple).template As<eastl::tuple_element_t<Index, tuple_args_t>>();
}

template<typename RebinderType>
CtorInfo::CtorInfo(RebinderType)
	: owner_type_{Typeof<typename RebinderType::owner_t>()}, function_{&RebinderType::Invoke}, id_{RebinderType::ID},
	  total_param_count_{RebinderType::args_t::SIZE}, optional_param_count_{RebinderType::OPTIONAL_PARAM_COUNT}
{
	static_assert(RebinderType::args_t::SIZE < 8, "Parameters signature count above allowed. Max is 10.");
	using tuple_t = typename TypeTraits::TlToTuple<typename RebinderType::args_t>::type_t;

	Detail::ApplyParamsSignature<0, RebinderType::args_t::SIZE, tuple_t>(params_signature_);
}

} // namespace Meta

#define META_REBINDER_CTOR(INDEX)	\
public:                                                                                                                \
	static constexpr auto Ctor_##INDEX##_ID = "Ctor_"#INDEX##_fnv1a;                                                                    \
                                                                                                                       \
private:                                                                                                               \
	friend struct Meta::CtorInfo::Rebinder<Ctor_##INDEX##_ID>

#define META_CTOR_INFO_BINDER(OWNER, INDEX, OPTIONAL_ARGS, ...)                                                        \
	template<> struct CtorInfo::Rebinder<OWNER::Ctor_##INDEX##_ID>                                                                \
		: Binder<OWNER::Ctor_##INDEX##_ID, OWNER(__VA_ARGS__), OPTIONAL_ARGS>

#define META_CTOR_INFO_BINDER_INVOKE_CUSTOM(...)                                                                       \
	static void Invoke(body_t& body)                                                                                   \
	{                                                                                                                  \
		__VA_ARGS__                                                                                                    \
	}

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS0()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(if (body.args_tuple_size == 0) { new (body.memory) owner_t{}; })

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS1()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {           \
		new (body.memory) owner_t{l_body.Get<0>()};                                                                    \
	})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS2()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS3()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS4()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 4) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>()};             \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS5()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 4) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>()};             \
			return;                                                                                                    \
		} if (body.args_tuple_size == 5) {                                                                             \
			new (body.memory)                                                                                          \
				owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(), l_body.Get<4>()};          \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS6()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 4) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>()};             \
			return;                                                                                                    \
		} if (body.args_tuple_size == 5) {                                                                             \
			new (body.memory)                                                                                          \
				owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(), l_body.Get<4>()};          \
			return;                                                                                                    \
		} if (body.args_tuple_size == 6) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(),                               \
									  l_body.Get<3>(), l_body.Get<4>(), l_body.Get<5>()};                              \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS7()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 4) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>()};             \
			return;                                                                                                    \
		} if (body.args_tuple_size == 5) {                                                                             \
			new (body.memory)                                                                                          \
				owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(), l_body.Get<4>()};          \
			return;                                                                                                    \
		} if (body.args_tuple_size == 6) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(),                               \
									  l_body.Get<3>(), l_body.Get<4>(), l_body.Get<5>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 7) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),              \
									  l_body.Get<4>(), l_body.Get<5>(), l_body.Get<6>()};                              \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS8()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 4) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>()};             \
			return;                                                                                                    \
		} if (body.args_tuple_size == 5) {                                                                             \
			new (body.memory)                                                                                          \
				owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(), l_body.Get<4>()};          \
			return;                                                                                                    \
		} if (body.args_tuple_size == 6) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(),                               \
									  l_body.Get<3>(), l_body.Get<4>(), l_body.Get<5>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 7) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),              \
									  l_body.Get<4>(), l_body.Get<5>(), l_body.Get<6>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 8) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),              \
									  l_body.Get<4>(), l_body.Get<5>(), l_body.Get<6>(), l_body.Get<7>()};             \
		})

#define META_CTOR_INFO_BINDER_DEFAULT(OWNER, INDEX, OPTIONAL_ARGS, ...)                                                \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_CTOR_INFO_BINDER(OWNER, INDEX, OPTIONAL_ARGS, __VA_ARGS__)                                                    \
	{                                                                                                                  \
		META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS0();                                                                  \
	};                                                                                                                 \
	}

#endif
