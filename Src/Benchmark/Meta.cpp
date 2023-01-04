
#define EASTL_ASSERT_ENABLED 0

#define BENCHMARK_STATIC_DEFINE

#define PRINT_ALLOCATIONS 0

#include "Meta/CtorInfo.h"

#include <benchmark/benchmark.h>

#include "Core/Common.h"
#include "Core/RawBuffer.h"

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
	mi_free_aligned(ptr, (uint64_t)al);
}

void operator delete[](void* ptr, std::align_val_t al)
{
	mi_free_aligned(ptr, (uint64_t)al);
}

static void TypeInfo_GetByTypename(benchmark::State& state)
{
	for (auto _ : state)
	{
		(void)Meta::Typeof<int64_t>();
	}
}

// Register the function as a benchmark
BENCHMARK(TypeInfo_GetByTypename);

static void TypeInfo_GetByName(benchmark::State& state)
{
	for (auto _ : state)
	{
		(void)Meta::Typeof("Int64");
	}
}

// Register the function as a benchmark
BENCHMARK(TypeInfo_GetByName);

static void EastlVector(benchmark::State& state)
{
	for (auto _ : state)
	{
		eastl::vector<float32_t> v{10, DEBUG_NAME_VAL("PlatformRender")};
		v.resize(100ull);
		v[0] = 3.14f;
		v[1] = 2.15f;
		v[2] = 1.16f;
	}
}

// Register the function as a benchmark
BENCHMARK(EastlVector);

static void COADRawBuffer(benchmark::State& state)
{
	for (auto _ : state)
	{
		RawBuffer<float32_t> v{10, DEBUG_NAME_VAL("PlatformRender")};
		v.Resize(100ull);
		v[0] = 3.14f;
		v[1] = 2.15f;
		v[2] = 1.16f;
	}
}

// Register the function as a benchmark
BENCHMARK(COADRawBuffer);

BENCHMARK_MAIN();

