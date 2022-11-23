
#define EASTL_ASSERT_ENABLED 0

#define BENCHMARK_STATIC_DEFINE

#define PRINT_ALLOCATIONS 0

#include "Meta/CtorInfo.h"

#include <benchmark/benchmark.h>

#include "Core/Common.h"

#include <EASTL/allocator_malloc.h>

#include "Core/entt.hpp"
#include "ECS/Registry.h"

#if _MSC_VER
#pragma comment(lib, "shlwapi")
#endif

using AllTransformComponentTypes = Ecs::TranformComponentTypes;
using AllComponentTypes = AllTransformComponentTypes;//TypeTraits::TlCat<AllTransformComponentTypes, Ecs::Placeholder64ComponentTypes>::Type;

int Vsnprintf8(char* pDestination, size_t n, const char* pFormat, va_list arguments)
{
	return vsnprintf(pDestination, n, pFormat, arguments);
}

void* operator new[](size_t size, const char* , int , unsigned , const char* , int )
{
	return mi_malloc(size);
}

void* operator new[](size_t size, size_t alignment, size_t , const char* , int , unsigned , const char* , int )
{
	return mi_aligned_alloc(size, alignment);
}

void operator delete(void* ptr)
{
	mi_free(ptr);
}

void operator delete[](void* ptr)
{
	mi_free(ptr);
}
void operator delete(void* ptr, std::align_val_t al)
{
	mi_free_aligned(ptr, (Uint64)al);
}

void operator delete[](void* ptr, std::align_val_t al)
{
	mi_free_aligned(ptr, (Uint64)al);
}

static void TypeInfo_GetByTypename(benchmark::State& state)
{
	for (auto _ : state)
	{
		(void)Meta::Typeof<Int64>();
	}
}

// Register the function as a benchmark
BENCHMARK(TypeInfo_GetByTypename);
//BENCHMARK(TypeInfo_GetByTypename)->Threads(8);

static void TypeInfo_GetByName(benchmark::State& state)
{
	for (auto _ : state)
	{
		(void)Meta::Typeof("Int64");
	}
}

// Register the function as a benchmark
BENCHMARK(TypeInfo_GetByName);
//BENCHMARK(TypeInfo_GetByName)->Threads(8);

struct TestCtor
{
	TestCtor()
	{
		//printf("default ctor\n");
	}

	TestCtor(TestCtor&& other) NOEXCEPT: pi{other.pi}
	{
		//printf("move ctor %f - %f\n", pi, other.pi);
		other.pi = 0.f;
	}
	TestCtor& operator=(TestCtor&& other) NOEXCEPT
	{
		//printf("move assign\n");
		pi		 = other.pi;
		other.pi = 0.f;
		return *this;
	}
	TestCtor(const TestCtor&)			 = default;
	TestCtor& operator=(const TestCtor&) = default;
	~TestCtor()
	{
		//printf("dtor\n");
	}

	float pi = 3.14f;


private:
	friend struct Meta::TypeInfo::Rebinder<TestCtor>;
	friend struct TestCtor_Ctor0;
	friend struct TestCtor_Ctor1;
	friend struct TestCtor_Ctor2;
	friend struct TestCtor_Property_number_;

	int number_;

	TestCtor(const Int64 index, Float32 pi = 3.14f) : pi{pi}
	{
		//printf("Index: %lld - Pi: %f\n", index, pi);
	}

	TestCtor(eastl::string_view name)
	{
		//printf("Name: %s\n", name.data());
	}
};

META_TYPE_BINDER_DEFAULT_NO_TO_STRING(TestCtor)

namespace Meta
{
//
//struct TestCtor_Property_number_ : Meta::PropertyInfo::Binder<decltype(&TestCtor::number_)>
//{
//	static void Set(const body_t& body)
//	{
//		static_cast<TestCtor*>(body.owner)->number_ = *static_cast<type_t*>(body.value);
//	}
//
//	static void* Get(const body_t& body)
//	{
//		return &static_cast<TestCtor*>(body.owner)->number_;
//	}
};

struct TestCtor_Ctor0 : Meta::CtorInfo::Binder<TestCtor(), 0>
{
	static void Invoke(body_t& body)
	{
		new (body.memory) TestCtor{};
	}
};

struct TestCtor_Ctor1 : Meta::CtorInfo::Binder<TestCtor(Int64, Float32), 1>
{
	static void Invoke(body_t& body)
	{
		auto& l_args_tuple = *static_cast<eastl::tuple<Meta::Value, Meta::Value>*>(body.args_tuple);
		if(body.args_tuple_size == 1)
		{
			new (body.memory) TestCtor{eastl::get<0>(l_args_tuple).AsInt64()};
			return;
		}
		if(body.args_tuple_size == 2)
		{
			new (body.memory) TestCtor{eastl::get<0>(l_args_tuple).AsInt64(), eastl::get<1>(l_args_tuple).AsFloat32()};
			return;
		}
	}
};

struct TestCtor_Ctor2 : Meta::CtorInfo::Binder<TestCtor(eastl::string_view), 1>
{
	static void Invoke(body_t& body)
	{
		const auto l_body = body_adapter_t{body};
		new (body.memory) TestCtor{eastl::get<0>(l_body.args)};
	}
};

using test_ctor_ctors_t = TypeTraits::TypeList<TestCtor_Ctor0, TestCtor_Ctor1, TestCtor_Ctor2>;

static void CtorInfo_Default(benchmark::State& state)
{
	Meta::CtorInfo l_tc_ctor0 = TypeTraits::TlGetByIndex<test_ctor_ctors_t, 0>::type_t{};

	for (auto _ : state)
	{
		auto v = l_tc_ctor0.Invoke();
		(void)v;
	}
}

// Register the function as a benchmark
BENCHMARK(CtorInfo_Default);
BENCHMARK(CtorInfo_Default)->Threads(8);

static void CtorInfo_CustomWithOptionalParam(benchmark::State& state)
{
	Meta::CtorInfo l_tc_ctor1 = TypeTraits::TlGetByIndex<test_ctor_ctors_t, 1>::type_t{};

	for (auto _ : state)
	{
		auto v = l_tc_ctor1.Invoke(4ll);
		(void)v;
	}
}

// Register the function as a benchmark
BENCHMARK(CtorInfo_CustomWithOptionalParam);
BENCHMARK(CtorInfo_CustomWithOptionalParam)->Threads(8);

static void CtorInfo_CustomWithAllParams(benchmark::State& state)
{
	Meta::CtorInfo l_tc_ctor1 = TypeTraits::TlGetByIndex<test_ctor_ctors_t, 1>::type_t{};
	//char l_memory[32];
	for (auto _ : state)
	{
		auto v = l_tc_ctor1.Invoke(4ll, 1.543f);
		(void)v;
	}
}

// Register the function as a benchmark
BENCHMARK(CtorInfo_CustomWithAllParams);
BENCHMARK(CtorInfo_CustomWithAllParams)->Threads(8);

//static void CtorInfo_Resolver(benchmark::State& state)
//{
//	/*Meta::CtorInfo l_tc_ctors[] =
//	{
//		Meta::CtorBinderOld<TestCtor, 0, 0>{},
//		Meta::CtorBinderOld<TestCtor, 1, 1>{}
//	};
//
//	for (auto _ : state)
//	{
//		auto v = Meta::CtorInfo::InvokeSolver<TestCtor>(eastl::span{l_tc_ctors}, 4ll);
//		(void)v;
//	}*/
//}
//
//// Register the function as a benchmark
//BENCHMARK(CtorInfo_Resolver);
//BENCHMARK(CtorInfo_Resolver)->Threads(8);

BENCHMARK_MAIN();

