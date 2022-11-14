
#define EASTL_ASSERT_ENABLED 0
#define COR_ENABLE_PROFILING 0
#define CORECLR_ENABLE_PROFILING 0
#define PRINT_ALLOCATIONS 1

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

#include <thread>
#include <chrono>
#include <iostream>

#include "Core/Log.h"
#include "ECS/Component.h"
#include "ECS/Registry.h"

#include <glm/gtx/string_cast.hpp>

int Vsnprintf8 (char*  pDestination, size_t n, const char*  pFormat, va_list arguments)
{
	return vsnprintf(pDestination, n, pFormat, arguments);
}

template < typename ... TypeLists >
struct ecs
{
	using ComponentTypes = typename TypeTraits::TlCat<TypeLists..., TypeTraits::TypeListEmpty>::Type;
};

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;
	LOG_SET_VERBOSITY(info);

	using T1 = TypeTraits::TypeList<int, float, bool>;
	using T2 = TypeTraits::TypeList<char*, void*, short>;

	ecs<Ecs::TranformComponentTypes>::ComponentTypes::SIZE;

	//printf("%s\n", componentInfo<LocationComponent>().ToString().c_str());

	Ecs::Registry<Ecs::TranformComponentTypes> l_reg{};
	auto l_id = l_reg.Create();

	//RESULT_ENFORCE_CALL(const auto l_id = l_reg.create());
	//RESULT_ENFORCE_CALL(const auto l_id2 = l_reg.create());
	//RESULT_ENFORCE_CALL(const auto l_id3 = l_reg.create());

	//l_reg.enable<LocationComponent>(l_id);

	//l_reg.add(l_id, LocationComponent{glm::vec3{2.f}});
	//l_reg.add(l_id2, RotationComponent{glm::vec3{5.f}});
	//l_reg.add(l_id3, ScaleComponent{glm::vec3{5.f}});

	//RESULT_ENFORCE_CALL(
	//	auto l_comp = l_reg.get<RotationComponent>(l_id2));

	//RotationComponent l_r2{};
	//RotationComponent* l_r {};

	//PTRA(l_comp);
	//PTRC(l_comp)->value.x = 3.f;

	//RESULT_ENFORCE_CALL(l_reg.destroy(l_id2));

	//const auto l_id4 = l_reg.create();
	//ASSERT(!l_reg.isEnabled<RotationComponent>(l_id4));
	
	return 0;
}
