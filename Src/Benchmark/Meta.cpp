
#define EASTL_ASSERT_ENABLED 0

#define BENCHMARK_STATIC_DEFINE

#define PRINT_ALLOCATIONS 0

#include "Core/Meta.h"

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

	TestCtor(const Int64 index, Float32 pi = 3.14f) : pi{pi}
	{
		//printf("Index: %lld - Pi: %f\n", index, pi);
	}

	TestCtor(TestCtor&& other)NOEXCEPT : pi{other.pi}
	{
		//printf("move ctor %f - %f\n", pi, other.pi);
		other.pi = 0.f;
	}
	TestCtor& operator=(TestCtor&& other)NOEXCEPT
	{
		//printf("move assign\n");
		pi = other.pi;
		other.pi = 0.f;
		return *this;
	}
	TestCtor(const TestCtor&)=delete;
	TestCtor& operator=(const TestCtor&)=delete;

	float pi = 3.14f;

private:
	friend struct Meta::CtorBinderOld<TestCtor, 0, 0>;
	friend struct Meta::CtorBinderOld<TestCtor, 1, 1>;
};


namespace Meta
{
template<> struct CtorBinderOld<TestCtor, 0, 0>
{
	static constexpr Uint32 OPTIONAL_PARAM_COUNT = 0;
	using ParameterTypes						 = TypeTraits::TypeList<>;
	using OwnerType								 = TestCtor;

	static void Exec(CtorInfo::Structure& structure)
	{
		new (structure.memory) TestCtor{};
	}
};
template<> struct CtorBinderOld<TestCtor, 1, 1>
{
	static constexpr Uint32 OPTIONAL_PARAM_COUNT = 1;
	using ParameterTypes						 = TypeTraits::TypeList<Int64, Float32>;
	using OwnerType								 = TestCtor;

	static void Exec(CtorInfo::Structure& structure)
	{
		CtorInfo::BodyAdapter<ParameterTypes> l_body{structure};
		if (l_body.provided_args_count == 1)
		{
			new (structure.memory) TestCtor{eastl::get<0>(l_body.args)};
		}
		else if (l_body.provided_args_count == 2)
		{
			new (structure.memory) TestCtor{eastl::get<0>(l_body.args), eastl::get<1>(l_body.args)};
		}
	}
};
} // namespace Meta

static void CtorInfo_Default(benchmark::State& state)
{
	Meta::CtorInfo l_tc_ctor0 = Meta::CtorBinderOld<TestCtor, 0, 0>{};

	for (auto _ : state)
	{
		auto v = l_tc_ctor0.Invoke<TestCtor>();
		(void)v;
	}
}

// Register the function as a benchmark
BENCHMARK(CtorInfo_Default);
BENCHMARK(CtorInfo_Default)->Threads(8);

static void CtorInfo_CustomWithOptionalParam(benchmark::State& state)
{
	Meta::CtorInfo l_tc_ctor1 = Meta::CtorBinderOld<TestCtor, 1, 1>{};

	for (auto _ : state)
	{
		auto v = l_tc_ctor1.Invoke<TestCtor>(4ll);
		(void)v;
	}
}

// Register the function as a benchmark
BENCHMARK(CtorInfo_CustomWithOptionalParam);
BENCHMARK(CtorInfo_CustomWithOptionalParam)->Threads(8);

static void CtorInfo_CustomWithAllParams(benchmark::State& state)
{
	Meta::CtorInfo l_tc_ctor1 = Meta::CtorBinderOld<TestCtor, 1, 1>{};

	for (auto _ : state)
	{
		auto v = l_tc_ctor1.Invoke<TestCtor>(4ll, 1.543f);
		(void)v;
	}
}

// Register the function as a benchmark
BENCHMARK(CtorInfo_CustomWithAllParams);
BENCHMARK(CtorInfo_CustomWithAllParams)->Threads(8);

static void CtorInfo_Resolver(benchmark::State& state)
{
	Meta::CtorInfo l_tc_ctors[] =
	{
		Meta::CtorBinderOld<TestCtor, 0, 0>{},
		Meta::CtorBinderOld<TestCtor, 1, 1>{}
	};

	for (auto _ : state)
	{
		auto v = Meta::CtorInfo::InvokeSolver<TestCtor>(eastl::span{l_tc_ctors}, 4ll);
		(void)v;
	}
}

// Register the function as a benchmark
BENCHMARK(CtorInfo_Resolver);
BENCHMARK(CtorInfo_Resolver)->Threads(8);

BENCHMARK_MAIN();

