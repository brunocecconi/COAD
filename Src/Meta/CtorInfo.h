
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

template<typename T>
constexpr bool CompareTypeInfoArray(const T* arr1, const T* arr2, const Uint32 size)
{
	for (Uint32 i = 0; i < size; i++)
	{
		if (*arr1[i] != *arr2[i])
		{
			return false;
		}
	}
	return true;
}

template<typename Array, typename Tuple>
constexpr Array GetParamsSignatureFromTuple(Tuple& tuple)
{
	Array l_array{};
	Detail::GetParamsSignatureFromTuple<0, Tuple>(l_array, tuple);
	return l_array;
}

template<typename T, typename... Args>
T* FindCompatibleCtor(const eastl::span<T>& ctors)
{
	const auto& l_args_signature_array = GetParamsSignatureFromTuple<eastl::tuple<Args...>>();

	for (auto l_it = ctors.rbegin(); l_it != ctors.rend(); ++l_it)
	{
		const auto& l_ctor_signature_array = l_it->ParamsSignature();

		if (CompareTypeInfoArray(l_ctor_signature_array.data(), l_args_signature_array.data(),
								 l_it->NeededParamCount()))
		{
			return &*l_it;
		}
	}
	return nullptr;
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
		const void*  info;
		void*  memory;
		Uint32 args_tuple_size;
		void*  args_tuple;
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
		using body_t		   = Body;
		using arguments_type_t = typename TypeTraits::TlToTuple<TypeList>::type_t;

		EXPLICIT BodyAdapter(body_t& structure);

		const CtorInfo&		  info;
		void*			  memory;
		Uint32&			  args_tuple_count;
		arguments_type_t& args_tuple;
	};

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
	template<typename CtorFunction, Uint32 OptionalParamCount>
	struct Binder
	{
		using owner_t				= typename TypeTraits::FunctionTraits<CtorFunction>::return_t;
		using parameter_type_list_t = typename TypeTraits::FunctionTraits<CtorFunction>::param_type_list_t;
		using body_t				= Body;
		using body_adapter_t		= BodyAdapter<parameter_type_list_t>;

		static constexpr Uint32 OPTIONAL_PARAM_COUNT = OptionalParamCount;
	};
	using binder_invoke_function_t = void (*)(Body&);

	/**
	 * @brief Invoke solver.
	 *
	 * WARNING: Can be slow because it has iterations based on a span of ctors.
	 *
	 * @tparam T Target type.
	 * @tparam ...Args Target argument types.
	 * @param ctors Span of ctors.
	 * @param ...args Target arguments.
	 *
	 * @return Type object.
	 */
	template<typename T, typename... Args>
	static T InvokeSolver(const eastl::span<CtorInfo>& ctors, Args&&... args);

	/**
	 * @brief Invoke solver generic.
	 *
	 * WARNING: Can be slow because it has iterations based on a span of ctors.
	 *
	 * @tparam T Target type.
	 * @tparam ...Args Target argument types.
	 * @param ctors Span of ctors.
	 * @param memory Target memory.
	 * @param ...args Target arguments.
	 *
	 * @return Type object.
	 *
	 */
	template<typename T, typename... Args>
	static T InvokeSolverGeneric(const eastl::span<CtorInfo>& ctors, void* memory, Args&&... args);

	template<typename BinderType>
	EXPLICIT CtorInfo(BinderType);

public:
	NODISCARD const TypeInfo&				   OwnerType() const;
	NODISCARD const method_params_signature_t& ParamsSignature() const;
	NODISCARD Uint32						   NeededParamCount() const;
	NODISCARD Uint32						   TotalParamCount() const;
	NODISCARD Uint32						   OptionalParamCount() const;

public:
	Value Invoke() const;
	Value Invoke(Value p1) const;
	Value Invoke(Value p1, Value p2) const;
	Value Invoke(Value p1, Value p2, Value p3) const;
	Value Invoke(Value p1, Value p2, Value p3, Value p4) const;
	Value Invoke(Value p1, Value p2, Value p3, Value p4, Value p5) const;
	Value Invoke(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const;
	Value Invoke(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7) const;
	Value Invoke(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7, Value p8) const;

private:
	const TypeInfo&			  owner_type_;
	method_params_signature_t params_signature_{};
	binder_invoke_function_t  function_;
	Uint32					  total_param_count_;
	Uint32					  optional_param_count_;
};

template<typename TypeList>
CtorInfo::BodyAdapter<TypeList>::BodyAdapter(body_t& structure)
	: info{*static_cast<const CtorInfo*>(structure.info)}, memory{structure.memory},
	  args_tuple_count{structure.args_tuple_size}, args_tuple{*static_cast<arguments_type_t*>(structure.args_tuple)}
{
}

template<typename T, typename... Args>
T CtorInfo::InvokeSolver(const eastl::span<CtorInfo>& ctors, Args&&... args)
{
	auto l_ctor = Detail::FindCompatibleCtor<CtorInfo, Args...>(ctors);
	ENFORCE_MSG(l_ctor, "Failed to find compatible ctor.");
	return l_ctor->template Invoke<T>(eastl::forward<Args>(args)...);
}

template<typename T, typename... Args>
T CtorInfo::InvokeSolverGeneric(const eastl::span<CtorInfo>& ctors, void* memory, Args&&... args)
{
	auto l_ctor = Detail::FindCompatibleCtor<CtorInfo, Args...>(ctors);
	ENFORCE_MSG(l_ctor, "Failed to find compatible ctor.");
	return l_ctor->template Invoke<T>(memory, eastl::forward<Args>(args)...);
}

template<typename BinderType>
CtorInfo::CtorInfo(BinderType)
	: owner_type_{Typeof<typename BinderType::owner_t>()}, function_{&BinderType::Invoke},
	  total_param_count_{BinderType::parameter_type_list_t::SIZE}, optional_param_count_{
																	   BinderType::OPTIONAL_PARAM_COUNT}
{
	static_assert(BinderType::parameter_type_list_t::SIZE < 8, "Parameters signature count above allowed. Max is 10.");
	using tuple_t = typename TypeTraits::TlToTuple<typename BinderType::parameter_type_list_t>::type_t;

	Detail::ApplyParamsSignature<0, BinderType::parameter_type_list_t::SIZE, tuple_t>(params_signature_);
}

} // namespace Meta

#endif
