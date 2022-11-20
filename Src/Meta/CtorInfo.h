
#ifndef META_CTOR_INFO_H
#define META_CTOR_INFO_H

#include "Meta/TypeInfo.h"

namespace Meta
{

static constexpr Uint32 METHOD_MAX_PARAMS_COUNT = 8;

using method_params_signature_t = eastl::array<const TypeInfo*, METHOD_MAX_PARAMS_COUNT>;

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
constexpr void GetParamsSignatureFromTuple(eastl::array<const TypeInfo*, ArraySize>& arr)
{
	if constexpr (Index < eastl::tuple_size_v<Tuple>)
	{
		arr[Index] = &Typeof<eastl::tuple_element_t<Index, Tuple>>();
		GetParamsSignatureFromTuple<Index + 1, Tuple>(arr);
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

template<typename Tuple>
constexpr method_params_signature_t GetParamsSignatureFromTuple()
{
	method_params_signature_t l_array{};
	Detail::GetParamsSignatureFromTuple<0, Tuple>(l_array);
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
	/**
	 * @brief Body structure.
	 *
	 * Used to pass information from the invoker function.
	 *
	 */
	struct Body
	{
		void*  ctor_info;
		void*  memory;
		Uint32 provided_args_count;
		void*  args;
	};

	using function_t = void (*)(Body&);

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

		const CtorInfo&									  ctor_info;
		void*											  memory;
		const Uint32&									  provided_args_count;
		typename TypeTraits::TlToTuple<TypeList>::type_t& args;
	};

	/**
	 * @brief Ctor binder.
	 *
	 * Used to apply information about a ctor and to implement the @ref Exec function that
	 * will be properly call type constructor.
	 *
	 * @tparam CtorFunction Ctor function type.
	 * @tparam OptionalParamCount Number of optional arguments. Later used for heuristics.
	 * @tparam Function Invocation function value.
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
	CtorInfo(BinderType);

public:
	NODISCARD const TypeInfo&				   OwnerType() const;
	NODISCARD const method_params_signature_t& ParamsSignature() const;
	NODISCARD Uint32						   NeededParamCount() const;
	NODISCARD Uint32						   TotalParamCount() const;
	NODISCARD Uint32						   OptionalParamCount() const;

public:
	template<typename T, typename... Args>
	T Invoke(Args&&... args);

	template<typename T, typename... Args>
	T InvokeGeneric(void* memory, Args&&... args);

private:
	const TypeInfo&			  owner_type_;
	method_params_signature_t params_signature_{};
	function_t				  function_;
	Uint32					  total_param_count_;
	Uint32					  optional_param_count_;
};

template<typename T>
TypeInfo::TypeInfo(TypeTag<T>)
	: id_{Hash::Fnv1AHash(Detail::TypeName<T>().size(), Detail::TypeName<T>().data())}, size_{sizeof(T)},
	  name_{Detail::TypeName<T>().data()}
{
	printf("'%s' type register: id=%llu, size=%llu\n", Detail::TypeName<T>().data(), id_, size_);
}

template<typename T>
const TypeInfo& TypeRegister::Emplace()
{
	constexpr auto l_type_name = Detail::TypeName<T>();
	auto		   l_it		   = types_.find(l_type_name.data());
	if (l_it == types_.cend())
	{
		l_it = types_.emplace(l_type_name.data(), TypeInfo{TypeTag<T>{}}).first;
	}
	return l_it->second;
}

template<typename TypeList>
CtorInfo::BodyAdapter<TypeList>::BodyAdapter(body_t& structure)
	: ctor_info{*static_cast<CtorInfo*>(structure.ctor_info)}, memory{structure.memory},
	  provided_args_count{structure.provided_args_count}, args{*static_cast<arguments_type_t*>(structure.args)}
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
	return l_ctor->template InvokeGeneric<T>(memory, eastl::forward<Args>(args)...);
}

template<typename BinderType>
CtorInfo::CtorInfo(BinderType)
	: owner_type_{Typeof<typename BinderType::owner_t>()}, function_{&BinderType::Exec},
	  total_param_count_{BinderType::parameter_type_list_t::SIZE}, optional_param_count_{
																	   BinderType::OPTIONAL_PARAM_COUNT}
{
	static_assert(BinderType::parameter_type_list_t::SIZE < METHOD_MAX_PARAMS_COUNT,
				  "Parameters signature count above allowed. Max is 10.");
	using tuple_t = typename TypeTraits::TlToTuple<typename BinderType::parameter_type_list_t>::type_t;

	Detail::ApplyParamsSignature<0, BinderType::parameter_type_list_t::SIZE, tuple_t>(params_signature_);
}

template<typename T, typename... Args>
T CtorInfo::Invoke(Args&&... args)
{
	char l_memory[sizeof(T)] = {};
	return InvokeGeneric<T>(l_memory, eastl::forward<Args>(args)...);
}

template<typename T, typename... Args>
T CtorInfo::InvokeGeneric(void* memory, Args&&... args)
{
	using args_type_t					 = eastl::tuple<Args...>;
	constexpr auto l_provided_args_count = sizeof...(Args);

	ENFORCE_MSG(Typeof<T>() == owner_type_, "Invalid owner type '%s'. Expected '%s'.", Detail::TypeName<T>().data(),
				owner_type_.Name());

	ENFORCE_MSG(l_provided_args_count >= NeededParamCount() && l_provided_args_count <= TotalParamCount(),
				"Invalid provided arguments count.");

	ENFORCE_MSG(CompareTypeInfoArray(ParamsSignature().data(),
									 Detail::GetParamsSignatureFromTuple<args_type_t>().data(), l_provided_args_count),
				"Invalid arguments signature.");

	auto l_args_tuple = eastl::make_tuple(eastl::forward<Args>(args)...);
	Body l_structure{static_cast<void*>(this), memory, sizeof...(Args), &l_args_tuple};
	function_(l_structure);
	return eastl::move(*static_cast<T*>(memory));
}

}

#endif
