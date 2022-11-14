
#define EASTL_ASSERT_ENABLED 0

#define BENCHMARK_STATIC_DEFINE

#define PRINT_ALLOCATIONS 0

#include <benchmark/benchmark.h>

#include "core/common.h"

#include <EASTL/allocator_malloc.h>

#include "core/entt.hpp"
#include "ecs/registry.h"

#if _MSC_VER
#pragma comment(lib, "shlwapi")
#endif

void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return mi_malloc(size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
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
	mi_free_aligned(ptr, (u64)al);
}

void operator delete[](void* ptr, std::align_val_t al)
{
	mi_free_aligned(ptr, (u64)al);
}

static void EnttCtor10000(benchmark::State& state)
{
	for (auto _ : state)
	{
		entt::registry l_reg{10000ull};
	}
}
// Register the function as a benchmark
BENCHMARK(EnttCtor10000);

static void EnttClear10000(benchmark::State& state)
{
	for (auto _ : state)
	{
		entt::registry l_reg{10000};
		l_reg.clear();
		//l_reg.reserve(10000);
	}
}
// Register the function as a benchmark
BENCHMARK(EnttClear10000);

static void COADEntityCtor10000(benchmark::State& state)
{
	for (auto _ : state)
	{
		Ecs::Registry l_reg{10000ull};
	}
}
// Register the function as a benchmark
BENCHMARK(COADEntityCtor10000);

static void COADEntityClear10000(benchmark::State& state)
{
	for (auto _ : state)
	{
		Ecs::Registry l_reg{10000};
		l_reg.Clear();
	}
}
// Register the function as a benchmark
BENCHMARK(COADEntityClear10000);

static void EnttCreateEntity1000ForLoop(benchmark::State& state)
{
	for (auto _ : state)
	{
		entt::registry l_reg{10000ull};
		for (size_t i = 0; i < 1000; i++)
		{
			(void)l_reg.create();
		}
	}
}
// Register the function as a benchmark
BENCHMARK(EnttCreateEntity1000ForLoop);

static void EnttCreateEntity1000ForLoopAddLocationComponent(benchmark::State& state)
{
	for (auto _ : state)
	{
		entt::registry l_reg{10000ull};
		for (size_t i = 0; i < 1000; i++)
		{
			const auto l_id = l_reg.create();
			l_reg.emplace<LocationComponent>(l_id, glm::vec3{static_cast<f32>(i)});
			//l_reg.emplace<RotationComponent>(l_id, glm::vec3{static_cast<f32>(i)*2});
		}
	}
}
// Register the function as a benchmark
BENCHMARK(EnttCreateEntity1000ForLoopAddLocationComponent);

static void COADEntityCreateEntity1000ForLoop(benchmark::State& state)
{
	for (auto _ : state)
	{
		Ecs::Registry l_reg{10000ull};
		for (size_t i = 0; i < 1000; i++)
		{
			(void)l_reg.Create();
		}
	}
}
// Register the function as a benchmark
BENCHMARK(COADEntityCreateEntity1000ForLoop);

static void COADEntityCreateEntity1000ForLoopAddLocationComponent(benchmark::State& state)
{
	for (auto _ : state)
	{
		Ecs::Registry l_reg{10000ull};
		for (size_t i = 0; i < 1000; i++)
		{
			const auto l_id = l_reg.Create();
			l_reg.Add(l_id, LocationComponent{glm::vec3{static_cast<f32>(i)}});
		}
	}
}
// Register the function as a benchmark
BENCHMARK(COADEntityCreateEntity1000ForLoopAddLocationComponent);

BENCHMARK_MAIN();

