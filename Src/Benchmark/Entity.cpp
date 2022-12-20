
#define EASTL_ASSERT_ENABLED 0

#define BENCHMARK_STATIC_DEFINE

#define PRINT_ALLOCATIONS 0

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
	mi_free_aligned(ptr, (uint64_t)al);
}

void operator delete[](void* ptr, std::align_val_t al)
{
	mi_free_aligned(ptr, (uint64_t)al);
}

static void EnttCtor10000(benchmark::State& state)
{
	for (auto _ : state)
	{
		entt::registry l_reg{10000ull};
	}
}
// Register the function as a benchmark
BENCHMARK(EnttCtor10000)->Threads(1);

static void COADEntityCtor10000(benchmark::State& state)
{
	for (auto _ : state)
	{
		Ecs::Registry<AllComponentTypes> l_reg{10000ull};
	}
}
// Register the function as a benchmark
BENCHMARK(COADEntityCtor10000)->Threads(1);

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
BENCHMARK(EnttClear10000)->Threads(1);

static void COADEntityClear10000(benchmark::State& state)
{
	for (auto _ : state)
	{
		Ecs::Registry<AllComponentTypes> l_reg{10000};
		l_reg.Clear();
	}
}
// Register the function as a benchmark
BENCHMARK(COADEntityClear10000)->Threads(1);

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
BENCHMARK(EnttCreateEntity1000ForLoop)->Threads(1);

static void COADEntityCreateEntity1000ForLoop(benchmark::State& state)
{
	for (auto _ : state)
	{
		Ecs::Registry<AllComponentTypes> l_reg{10000ull};
		for (size_t i = 0; i < 1000; i++)
		{
			(void)l_reg.Create();
		}
	}
}
// Register the function as a benchmark
BENCHMARK(COADEntityCreateEntity1000ForLoop)->Threads(1);

static void EnttCreateEntity1000ForLoopAddLocationComponent(benchmark::State& state)
{
	for (auto _ : state)
	{
		entt::registry l_reg{10000ull};
		for (size_t i = 0; i < 1000; i++)
		{
			const auto l_id = l_reg.create();
			l_reg.emplace<Ecs::LocationComponent>(l_id, glm::vec3{static_cast<float32_t>(i)});
			//l_reg.emplace<RotationComponent>(l_id, glm::vec3{static_cast<Float32>(i)*2});
		}
	}
}
// Register the function as a benchmark
BENCHMARK(EnttCreateEntity1000ForLoopAddLocationComponent)->Threads(1);

static void COADEntityCreateEntity1000ForLoopAddLocationComponent(benchmark::State& state)
{
	for (auto _ : state)
	{
		Ecs::Registry<AllComponentTypes> l_reg{10000ull};
		for (size_t i = 0; i < 1000; i++)
		{
			const auto l_id = l_reg.Create();
			l_reg.Add(l_id, Ecs::LocationComponent{glm::vec3{static_cast<float32_t>(i)}});
		}
	}
}
// Register the function as a benchmark
BENCHMARK(COADEntityCreateEntity1000ForLoopAddLocationComponent)->Threads(1);

static void EnttDestroyEntity1000ForLoop(benchmark::State& state)
{
	for (auto _ : state)
	{
		entt::registry l_reg{10000ull};
		for (size_t i = 0; i < 1000; i++)
		{
			(void)l_reg.create();
		}
		for (size_t i = 0; i < 1000; i++)
		{
			l_reg.destroy((entt::registry::entity_type)i);
		}
	}
}
// Register the function as a benchmark
BENCHMARK(EnttDestroyEntity1000ForLoop)->Threads(1);

static void COADDestroyEntity1000ForLoop(benchmark::State& state)
{
	for (auto _ : state)
	{
		Ecs::Registry<AllComponentTypes> l_reg{10000ull};
		for (size_t i = 0; i < 1000; i++)
		{
			(void)l_reg.Create();
		}
		for (size_t i = 0; i < 1000; i++)
		{
			l_reg.Destroy(i);
		}
	}
}
// Register the function as a benchmark
BENCHMARK(COADDestroyEntity1000ForLoop)->Threads(1);

static void EnttRemoveComponent1000ForLoop(benchmark::State& state)
{
	for (auto _ : state)
	{
		entt::registry l_reg{10000ull};

		for (size_t i = 0; i < 1000; i++)
		{
			l_reg.emplace<Ecs::LocationComponent>(l_reg.create(), glm::vec3{static_cast<float32_t>(i)});
		}
		for (size_t i = 0; i < 1000; i++)
		{
			l_reg.remove<Ecs::LocationComponent>((entt::registry::entity_type)i);
		}
	}
}
// Register the function as a benchmark
BENCHMARK(EnttRemoveComponent1000ForLoop)->Threads(1);

static void COADRemoveComponent1000ForLoop(benchmark::State& state)
{
	for (auto _ : state)
	{
		Ecs::Registry<AllComponentTypes> l_reg{10000ull};
		for (size_t i = 0; i < 1000; i++)
		{
			l_reg.Add(l_reg.Create(), Ecs::LocationComponent{glm::vec3{static_cast<float32_t>(i)}});
		}
		for (size_t i = 0; i < 1000; i++)
		{
			l_reg.Remove<Ecs::LocationComponent>(i);
		}
	}
}
// Register the function as a benchmark
BENCHMARK(COADRemoveComponent1000ForLoop)->Threads(1);

#pragma optimize( "", off )
void nop(){}
#pragma optimize( "", on )

static void EnttGetEntity10000ForLoop(benchmark::State& state)
{
	entt::registry l_reg{10000ull};
	for (size_t i = 0; i < 10000; i++)
	{
		l_reg.emplace<Ecs::LocationComponent>(l_reg.create(), glm::vec3{static_cast<float32_t>(i)});
	}
	for (auto _ : state)
	{
		for (size_t i = 0; i < 10000; i++)
		{
			nop();
			(void)l_reg.get<Ecs::LocationComponent>((entt::registry::entity_type)i);
		}
	}
}
// Register the function as a benchmark
BENCHMARK(EnttGetEntity10000ForLoop)->Threads(1);

static void COADGetEntity10000ForLoop(benchmark::State& state)
{
	Ecs::Registry<AllComponentTypes> l_reg{10000ull};
	for (size_t i = 0; i < 10000; i++)
	{
		l_reg.Add(l_reg.Create(), Ecs::LocationComponent{glm::vec3{static_cast<float32_t>(i)}});
	}
	for (auto _ : state)
	{
		for (size_t i = 0; i < 10000; i++)
		{
			nop();
			(void)l_reg.Get<Ecs::LocationComponent>(i);
		}
	}
}
// Register the function as a benchmark
BENCHMARK(COADGetEntity10000ForLoop)->Threads(1);

static void EnttEach10000(benchmark::State& state)
{
	entt::registry l_reg{10000ull};
	for (size_t i = 0; i < 10000; i++)
	{
		l_reg.emplace<Ecs::LocationComponent>(l_reg.create(), glm::vec3{static_cast<float32_t>(i)});
	}
	for (auto _ : state)
	{
		l_reg.each([&](auto& d)
		{
			(void)l_reg.get<Ecs::LocationComponent>(d);
		});
	}
}
// Register the function as a benchmark
BENCHMARK(EnttEach10000)->Threads(1);

static void COADEach10000(benchmark::State& state)
{
	Ecs::Registry<AllComponentTypes> l_reg{10000ull};
	for (size_t i = 0; i < 10000; i++)
	{
		l_reg.Add(l_reg.Create(), Ecs::LocationComponent{glm::vec3{static_cast<float32_t>(i)}});
	}
	for (auto _ : state)
	{
		l_reg.Each<Ecs::LocationComponent>([](auto& d)
		{
			d.value.x=3.14f;
		});
	}
}
// Register the function as a benchmark
BENCHMARK(COADEach10000)->Threads(1);

BENCHMARK_MAIN();

