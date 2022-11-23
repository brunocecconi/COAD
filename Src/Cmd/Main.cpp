
#define EASTL_ASSERT_ENABLED	 0
#define COR_ENABLE_PROFILING	 0
#define CORECLR_ENABLE_PROFILING 0
#define PRINT_ALLOCATIONS		 1
#define EASTL_EASTDC_VSNPRINTF	 0

#include <stdio.h>
#include "core/types.h"
#include <mimalloc.h>
#include <memory>
#include <algorithm>
#include "Core/IO.h"
#include "Core/Assert.h"
#include "Core/Thread.h"
#include "Core/Allocator.h"
#include "Core/IO.h"

#include <thread>
#include <chrono>
#include <iostream>

#include "Core/Log.h"
#include "ECS/Component.h"
#include "ECS/Registry.h"
#include "Core/Algorithm.h"

#include <glm/gtx/string_cast.hpp>

#include "Meta/ClassInfo.h"
#include "Meta/CtorInfo.h"
#include "Meta/EnumInfo.h"
#include "Meta/PropertyInfo.h"
#include "Meta/Value.h"
#include "Core/RawBuffer.h"
#include "Meta/MethodInfo.h"

int Vsnprintf8(char* pDestination, size_t n, const char* pFormat, va_list arguments)
{
	return vsnprintf(pDestination, n, pFormat, arguments);
}

struct TestCtor
{
	TestCtor()
	{
		printf("default ctor\n");
	}

	TestCtor(TestCtor&& other) NOEXCEPT: pi{other.pi}
	{
		printf("move ctor %f - %f\n", pi, other.pi);
		other.pi = 0.f;
	}
	TestCtor& operator=(TestCtor&& other) NOEXCEPT
	{
		printf("move assign\n");
		pi		 = other.pi;
		other.pi = 0.f;
		return *this;
	}
	TestCtor(const TestCtor&)			 = default;
	TestCtor& operator=(const TestCtor&) = default;
	~TestCtor()
	{
		printf("dtor\n");
	}

	float pi = 3.14f;

	NODISCARD bool IsEven(const Int32 value) const
	{
		return value % 2 == 0;
	}

private:
	friend struct Meta::TypeInfo::Rebinder<TestCtor>;
	friend struct TestCtor_Ctor0;
	friend struct TestCtor_Ctor1;
	friend struct TestCtor_Ctor2;
	friend struct TestCtor_Property_number_;
	friend struct TestCtor_Method_IsEven;

	int number_ = 6;

	TestCtor(const Int64 index, Float32 pi = 3.14f) : pi{pi}
	{
		printf("Index: %lld - Pi: %f\n", index, pi);
	}

	TestCtor(eastl::string_view name)
	{
		printf("Name: %s\n", name.data());
	}
};

namespace Meta                                                                                                     
{                                                                                                                  
META_TYPE_BINDER_SPECIALIZATION(TestCtor)
{
#undef TO_STRING
#define TO_STRING	\
auto& l_to_string_args_tuple = *static_cast<eastl::tuple<eastl::string*, TestCtor*, Uint64>*>(body.args_tuple);			\
		eastl::get<0>(l_to_string_args_tuple)->resize(256);	\
		sprintf(eastl::get<0>(l_to_string_args_tuple)->data(), "TestCtor(pi=%f)",	\
			eastl::get<1>(l_to_string_args_tuple)->pi);

	META_TYPE_BINDER_BODY(TestCtor)
	META_TYPE_BINDER_OPERATIONS_CUSTOM(              
	META_TYPE_BINDER_DEFAULT_OPERATION_DEFAULT_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_CTOR(),             
	META_TYPE_BINDER_DEFAULT_OPERATION_COPY_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_ASSIGN(),              
	META_TYPE_BINDER_DEFAULT_OPERATION_COPY_ASSIGN(), META_TYPE_BINDER_DEFAULT_OPERATION_DTOR(), 
		TO_STRING
	)
};          
}

struct TestCtor_Method_IsEven : Meta::MethodInfo::Binder<TestCtor, bool, TypeTraits::TypeList<Int32>>
{
	using base_t = Meta::MethodInfo::Binder<TestCtor, bool, TypeTraits::TypeList<Int32>>;

	static constexpr auto ID = Hash::Fnv1AHash("TestCtor::IsEven");

	constexpr TestCtor_Method_IsEven() : base_t{"IsEven"}
	{
	}

	static Meta::Value Invoke(body_t& body)
	{
		Meta::MethodInfo::BodyAdapter<TestCtor, args_t> l_body{body};
		return l_body.owner.IsEven(eastl::get<0>(l_body.args_tuple));
	}
};

struct TestCtor_Property_number_: Meta::PropertyInfo::Binder<TestCtor, int, Meta::PropertyInfo::eReadOnly>
{
	using base_t = Meta::PropertyInfo::Binder<TestCtor, int, Meta::PropertyInfo::eReadOnly>;

	static constexpr auto ID = Hash::Fnv1AHash("TestCtor::number_");

	constexpr TestCtor_Property_number_() : base_t{"number_"}
	{
	}

	static void Set(const body_t& body)
	{
		if constexpr (FLAGS & (Meta::PropertyInfo::eWriteOnly | Meta::PropertyInfo::eReadWrite))
		{
			static_cast<TestCtor*>(body.owner)->number_ = *static_cast<type_t*>(body.value);
		}
	}

	static void* Get(const body_t& body)
	{
		if constexpr (FLAGS & (Meta::PropertyInfo::eReadOnly | Meta::PropertyInfo::eReadWrite))
		{
			return &static_cast<TestCtor*>(body.owner)->number_;
		}
		else
		{
			return nullptr;
		}
	}
};

struct TestCtor_Ctor0: Meta::CtorInfo::Binder<TestCtor(), 0>
{
	static void Invoke(body_t& body)
	{
		new (body.memory) TestCtor{};
	}
};

struct TestCtor_Ctor1: Meta::CtorInfo::Binder<TestCtor(Int64, Float32), 1>
{
	static void Invoke(body_t& body)
	{
		auto& l_args_tuple = *static_cast<eastl::tuple<Meta::Value, Meta::Value>*>(body.args_tuple);
		if (body.args_tuple_size == 1)
		{
			new (body.memory) TestCtor{eastl::get<0>(l_args_tuple).AsInt64()};
			return;
		}
		if (body.args_tuple_size == 2)
		{
			new (body.memory) TestCtor{eastl::get<0>(l_args_tuple).AsInt64(), eastl::get<1>(l_args_tuple).AsFloat32()};
			return;
		}
	}
};

struct TestCtor_Ctor2: Meta::CtorInfo::Binder<TestCtor(eastl::string_view), 1>
{
	static void Invoke(body_t& body)
	{
		const auto l_body = body_adapter_t{body};
		new (body.memory) TestCtor{eastl::get<0>(l_body.args_tuple)};
	}
};

using test_ctor_ctors_t = TypeTraits::TypeList<TestCtor_Ctor0, TestCtor_Ctor1, TestCtor_Ctor2>;

namespace Meta
{

template<>
struct ClassInfo::Rebinder<TestCtor> : ClassInfo::Binder<TestCtor, 
	TypeTraits::TypeList<TestCtor_Ctor0, TestCtor_Ctor1, TestCtor_Ctor2>,
	TypeTraits::TypeList<TestCtor_Property_number_>,
	TypeTraits::TypeList<TestCtor_Method_IsEven>>
{
	static constexpr Hash::fnv1a_t ID = TypeInfo::Rebinder<TestCtor>::ID;
};

}

struct Test
{
};

enum TestEnum
{
	eNone,
	eFirst,
	eSecond,
	eThird
};

META_TYPE_BINDER_DEFAULT_NO_TO_STRING(TestEnum)

namespace Meta
{
template<>
struct EnumInfo::Rebinder<TestEnum>: Binder<TestEnum>
{
	static auto Pairs()
	{
		return eastl::vector<eastl::pair<eastl::string_view, Int64>>{{{"eNone", TestEnum::eNone},
																	  {"eFirst", TestEnum::eFirst},
																	  {"eSecond", TestEnum::eSecond},
																	  {"eThird", TestEnum::eThird}}};
	}
};
} // namespace Meta

struct ASDB
{
	char v[256];
};
META_TYPE_BINDER_DEFAULT_NO_TO_STRING(ASDB)

int main(int argc, char** argv)
{
#if USE_RESULT
	Result** result = nullptr;
	(void)result;
#endif

	(void)argc;
	(void)argv;
	LOG_SET_VERBOSITY(info);

	Meta::RegistryBaseTypes();

	LOG(warning, "#################");

	const Meta::Value l_v1 = 3.4f;
	Meta::Value		  l_v2 = l_v1;
	l_v2				   = 7;

	Meta::CtorInfo l_ctor1	{TypeTraits::TlGetByIndex<test_ctor_ctors_t, 1>::type_t{}};
	const auto	   l_result = l_ctor1.Invoke(4ll,5.f);

	printf("%f\n", l_result.As<TestCtor>().pi);

	Meta::MethodInfo l_check_int {TestCtor_Method_IsEven{}};
	Int32 l_n = 4;

	eastl::vector<int> d{3,4,5,6};

	if(const auto l_iseven = l_check_int.Invoke(l_result.AsGeneric(), l_n); l_iseven.AsBool())
	{
		printf("TestCtor::IsEven by reflection: number '%d' is even\n", l_n);
	}

	const auto& l_test_ctor_ci = Meta::Classof<TestCtor>();
	if(l_test_ctor_ci.HasMethodInfo("TestCtor::IsEven"))
	{
		const auto& l_method = l_test_ctor_ci.GetMethodInfo("TestCtor::IsEven");
		l_method.Invoke(nullptr, 4);
	}

	// const auto& l_test_enum_info = Meta::Enumof<TestEnum>();
	// TestEnum t = eSecond;

	// printf("%s\n", l_test_enum_info.ToString().c_str());

	/*Meta::CtorInfo l_ctor0 = TypeTraits::TlGetByIndex<test_ctor_ctors_t, 0>::type_t{};
	Meta::CtorInfo l_ctor1 = TypeTraits::TlGetByIndex<test_ctor_ctors_t, 1>::type_t{};
	Meta::CtorInfo l_ctor2 = TypeTraits::TlGetByIndex<test_ctor_ctors_t, 2>::type_t{};

	char l_mem[sizeof(TestCtor)];
	auto l_result = l_ctor0.InvokeGenericAny(l_mem);
	auto&& l_tc = AnyCast<TestCtor>(l_result);
	printf("%f\n", l_tc.pi);*/

	/*l_ctor0.Invoke<TestCtor>();
	l_ctor1.Invoke<TestCtor>(22ll);
	l_ctor2.Invoke<TestCtor>(eastl::string_view{"blabla"});*/

	/*Meta::CtorInfo l_tc_ctors[] = {Meta::CtorBinderOld<TestCtor, 0, 0>{}, Meta::CtorBinderOld<TestCtor, 1, 1>{},
								   Meta::CtorBinderOld<TestCtor, 2, 0>{}};

	char l_mem[sizeof(TestCtor)];
	Meta::CtorInfo::InvokeSolverGeneric<TestCtor>(eastl::span{l_tc_ctors}, l_mem, 5ll);
	Meta::CtorInfo::InvokeSolver<TestCtor>(eastl::span{l_tc_ctors}, 4ll, 1.432f);
	Meta::CtorInfo::InvokeSolver<TestCtor>(eastl::span{l_tc_ctors}, eastl::string_view{"lauro"});*/

	return 0;
}
