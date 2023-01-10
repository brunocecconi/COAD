
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
#include "Core/Io.h"
#include "Core/Assert.h"
#include "Core/Thread.h"
#include "Core/Allocator.h"
#include "Core/Io.h"

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
#include "Asset/Registry.h"
#include "Engine/Manager.h"
#include "Engine/ProgramArgs.h"

int Vsnprintf8(char* pDestination, size_t n, const char* pFormat, va_list arguments)
{
	return vsnprintf(pDestination, n, pFormat, arguments);
}

struct TestCtor
{
	CLASS_BODY_ONLY_HEADER(TestCtor)

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

	NODISCARD bool IsEven(const int32_t value) const
	{
		return value % 2 == 0;
	}

private:
	int number_ = 6;

	TestCtor(const int64_t index, float32_t pi = 3.14f) : pi{pi}
	{
		printf("Index: %lld - Pi: %f\n", index, pi);
	}

	TestCtor(eastl::string_view name)
	{
		printf("Name: %s\n", name.data());
	}

private:
	META_REBINDER_TYPE_INFO();
	META_REBINDER_CTOR(0);
	META_REBINDER_CTOR(1);
	META_REBINDER_CTOR(2);
	META_REBINDER_PROPERTY(TestCtor, number_);
	META_REBINDER_METHOD(TestCtor, IsEven);
};

namespace Meta
{
META_TYPE_BINDER_SPECIALIZATION(TestCtor)
{
#undef TO_STRING
#define TO_STRING                                                                                                      \
	auto& l_to_string_args_tuple = *static_cast<eastl::tuple<eastl::string*, TestCtor*, uint64_t>*>(body.args_tuple);  \
	eastl::get<0>(l_to_string_args_tuple)->resize(256);                                                                \
	sprintf(eastl::get<0>(l_to_string_args_tuple)->data(), "TestCtor(pi=%f)",                                          \
			eastl::get<1>(l_to_string_args_tuple)->pi);

	META_TYPE_BINDER_BODY(TestCtor)
	META_TYPE_BINDER_OPERATIONS_CUSTOM(
		META_TYPE_BINDER_DEFAULT_OPERATION_DEFAULT_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_CTOR(),
		META_TYPE_BINDER_DEFAULT_OPERATION_COPY_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_ASSIGN(),
		META_TYPE_BINDER_DEFAULT_OPERATION_COPY_ASSIGN(), META_TYPE_BINDER_DEFAULT_OPERATION_DTOR(), TO_STRING)
};
} // namespace Meta
META_TYPE_AUTO_REGISTER(TestCtor);

META_METHOD_INFO_BINDER_DEFAULT(TestCtor, IsEven, bool, 0, Meta::MethodInfo::eCallable,
								META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS1, int32_t);

META_PROPERTY_INFO_BINDER_DEFAULT(TestCtor, int, number_, Meta::PropertyInfo::eReadOnly);

namespace Meta
{
META_CTOR_INFO_BINDER(TestCtor, 0, 0)
// struct TestCtor_Ctor0: Meta::CtorInfo::Binder<TestCtor(), 0>
{
	META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS0();
};

META_CTOR_INFO_BINDER(TestCtor, 1, 1, int64_t, float32_t)
// struct TestCtor_Ctor1: Meta::CtorInfo::Binder<TestCtor(Int64, Float32), 1>
{
	META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS2();
};

META_CTOR_INFO_BINDER(TestCtor, 2, 0, eastl::string_view)
// struct TestCtor_Ctor2: Meta::CtorInfo::Binder<TestCtor(eastl::string_view), 1>
{
	META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS1();
};

} // namespace Meta

#undef META_CLASS_INFO_TARGET
#define META_CLASS_INFO_TARGET TestCtor
META_CLASS_INFO_BEGIN(0)
BINDERS(CTOR_BINDER(0)), BINDERS(PROPERTY_BINDER(number_)),
	BINDERS(METHOD_BINDER(IsEven)) META_CLASS_INFO_END()

enum TestEnum
{
	eNone,
	eFirst,
	eSecond,
	eThird
};

META_TYPE_BINDER_DEFAULT_NO_TO_STRING(TestEnum)

#undef META_ENUM_INFO_TARGET
#define META_ENUM_INFO_TARGET TestEnum
META_ENUM_INFO_BINDER_BEGIN(0)
ENUM_VALUE(eNone), ENUM_VALUE(eFirst), ENUM_VALUE(eSecond), ENUM_VALUE(eThird) META_ENUM_INFO_BINDER_END();

struct ASDB
{
	char v[256];
};
META_TYPE_BINDER_BEGIN(ASDB)
META_TYPE_BINDER_NO_OPERATIONS()
META_TYPE_BINDER_END()
META_TYPE_AUTO_REGISTER(ASDB);

LOG_DEFINE(Main);

int32_t ProcessGame(const int32_t Argc, char** Argv)
{
	RESULT_VALUE_VAR(result);
	LOG_SET_VERBOSITY(Info);
	LOGC(Warning, Main, "Preparing to run the game...");
	auto& lEngine = Engine::Manager::Instance();
	RESULT_VALUE_ENSURE_CALL(lEngine.Initialize(Argc, Argv, RESULT_ARG_VALUE_PASS), EXIT_FAILURE);
	RESULT_VALUE_ENSURE_CALL(lEngine.Run(RESULT_ARG_VALUE_PASS), EXIT_FAILURE);
	return EXIT_SUCCESS;
}

int32_t main(const int32_t Argc, char** Argv)
{
	LOG_INIT();
	if (const Engine::ProgramArgs lArgs{Argc, Argv}; lArgs.Has("--Game"))
	{
		return ProcessGame(Argc, Argv);
	}

	EResult result = Ok;
	(void)result;

	(void)Argc;
	(void)Argv;
	LOG_SET_VERBOSITY(Info);

	Meta::RegistryBaseTypes();

	LOG(Warning, "#################");

	const Meta::Value l_v1 = 3.4f;
	Meta::Value		  l_v2 = l_v1;
	l_v2				   = 7;

	eastl::vector<int> d{3, 4, 5, 6};

	const auto& l_info	  = Meta::Classof<TestCtor>();
	auto		l_tc	  = l_info.GetCtorInfo(0)();
	const auto& IsEventFI = l_info.GetMethodInfo("TestCtor::IsEven");
	const auto& NumberPI  = l_info.GetPropertyInfo("TestCtor::number_");
	NumberPI.Set(l_tc, 4);
	printf("%d\n", NumberPI.Get(l_tc).AsInt32());
	if (IsEventFI(l_tc, NumberPI.Get(l_tc)))
	{
		printf("EVEN\n");
	}

	// l_asset_reg.Import<Asset::Object>("./test.txt" RESULT_ARG_PASS);
	Asset::Registry::Instance(&RESULT_ARG_PASS);

	RESULT_VALUE_ENSURE_CALL(Asset::Registry::Instance().Load<Asset::Object>("./test.txt", &RESULT_ARG_PASS), -1);

	RESULT_VALUE_ENSURE_CALL(
		const auto& l_test_asset = Asset::Registry::Instance().Get<Asset::Object>("./test.txt", &RESULT_ARG_PASS), -1);
	(void)l_test_asset;
	printf("%s\n", l_test_asset->ToString(256).c_str());

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
