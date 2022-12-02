
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

struct PodClassType
{
  int a;
  float b;
};

FORCEINLINE void Construct(PodClassType& value, int a, float b)
{
  value.a = a;
  value.b = b;
}

FORCEINLINE void Destruct(PodClassType& value)
{
}

static void PodClass(benchmark::State& state) {
  for (auto _ : state) {
    PodClassType v;
    Construct(v, 4, 2.3f);
    Destruct(v);
  }
}
// Register the function as a benchmark
BENCHMARK(PodClass);

class DefaultClassType
{
public:
  DefaultClassType(int a, float b) : a{a}, b{b} {}

  int a;
  float b;
};

static void DefatulClass(benchmark::State& state) {
  for (auto _ : state) {
    DefaultClassType v{4, 2.3f};
  }
}
BENCHMARK(DefatulClass);

BENCHMARK_MAIN();

