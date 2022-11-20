
#define EASTL_ASSERT_ENABLED	 0
#define COR_ENABLE_PROFILING	 0
#define CORECLR_ENABLE_PROFILING 0
#define PRINT_ALLOCATIONS		 1

#include <stdio.h>
#include "core/types.h"
#include <mimalloc.h>
#include <memory>
#include <algorithm>
#include "Core/IO.h"
#include "Core/Assert.h"
#include "Core/Thread.h"
#include "Core/Allocator.h"
#include "Core/TypeInfo.h"
#include "Core/IO.h"

#include <thread>
#include <chrono>
#include <iostream>

#include "Core/Log.h"
#include "ECS/Component.h"
#include "ECS/Registry.h"
#include "Core/Meta.h"
#include "Core/Algorithm.h"

#include <glm/gtx/string_cast.hpp>

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
	TestCtor(const TestCtor&)			 = delete;
	TestCtor& operator=(const TestCtor&) = delete;
	~TestCtor()
	{
		printf("dtor\n");
	}

	float pi = 3.14f;

private:
	friend struct TestCtor_Ctor0;
	friend struct TestCtor_Ctor1;
	friend struct TestCtor_Ctor2;

	TestCtor(const Int64 index, Float32 pi = 3.14f) : pi{pi}
	{
		printf("Index: %lld - Pi: %f\n", index, pi);
	}

	TestCtor(eastl::string_view name)
	{
		printf("Name: %s\n", name.data());
	}
};

struct TestCtor_Ctor0 : Meta::CtorInfo::Binder<TestCtor(), 0>
{
	static void Exec(body_t& body)
	{
		new (body.memory) TestCtor{};
	}
};

struct TestCtor_Ctor1 : Meta::CtorInfo::Binder<TestCtor(Int64, Float32), 1>
{
	static void Exec(body_t& body)
	{
		const auto l_body = body_adapter_t{body};
		if(l_body.provided_args_count == 1)
		{
			new (body.memory) TestCtor{eastl::get<0>(l_body.args)};
			return;
		}
		if(l_body.provided_args_count == 2)
		{
			new (body.memory) TestCtor{eastl::get<0>(l_body.args), eastl::get<1>(l_body.args)};
			return;
		}
	}
};

struct TestCtor_Ctor2 : Meta::CtorInfo::Binder<TestCtor(eastl::string_view), 1>
{
	static void Exec(body_t& body)
	{
		const auto l_body = body_adapter_t{body};
		new (body.memory) TestCtor{eastl::get<0>(l_body.args)};
	}
};

using test_ctor_ctors_t = TypeTraits::TypeList<TestCtor_Ctor0, TestCtor_Ctor1, TestCtor_Ctor2>;

//namespace Meta
//{
//
//template<>
//struct CtorBinderOld<TestCtor, 1, 1>: CtorBinderBaseOld<TestCtor>
//{
//	static constexpr Uint32 OPTIONAL_PARAM_COUNT = 1;
//	using ParameterTypes						 = TypeTraits::TypeList<Int64, Float32>;
//
//	static void Exec(CtorInfo::Structure& structure)
//	{
//		
//	}
//};
//template<>
//struct CtorBinderOld<TestCtor, 2, 0>: CtorBinderBaseOld<TestCtor>
//{
//	using ParameterTypes = TypeTraits::TypeList<eastl::string_view>;
//
//	static void Exec(CtorInfo::Structure& structure)
//	{
//		CtorInfo::BodyAdapter<ParameterTypes> l_body{structure};
//		new (structure.memory) TestCtor{eastl::get<0>(l_body.args)};
//	}
//};
//
//} // namespace Meta

struct Test
{
};

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

	Meta::CtorInfo l_ctor0 = TypeTraits::TlGetByIndex<test_ctor_ctors_t, 0>::type_t{};
	Meta::CtorInfo l_ctor1 = TypeTraits::TlGetByIndex<test_ctor_ctors_t, 1>::type_t{};
	Meta::CtorInfo l_ctor2 = TypeTraits::TlGetByIndex<test_ctor_ctors_t, 2>::type_t{};

	l_ctor0.Invoke<TestCtor>();
	l_ctor1.Invoke<TestCtor>(22ll);
	l_ctor2.Invoke<TestCtor>(eastl::string_view{"blabla"});

	/*Meta::CtorInfo l_tc_ctors[] = {Meta::CtorBinderOld<TestCtor, 0, 0>{}, Meta::CtorBinderOld<TestCtor, 1, 1>{},
								   Meta::CtorBinderOld<TestCtor, 2, 0>{}};

	char l_mem[sizeof(TestCtor)];
	Meta::CtorInfo::InvokeSolverGeneric<TestCtor>(eastl::span{l_tc_ctors}, l_mem, 5ll);
	Meta::CtorInfo::InvokeSolver<TestCtor>(eastl::span{l_tc_ctors}, 4ll, 1.432f);
	Meta::CtorInfo::InvokeSolver<TestCtor>(eastl::span{l_tc_ctors}, eastl::string_view{"lauro"});*/

	return 0;
}
