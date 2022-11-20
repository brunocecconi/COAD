
/** \file Thread.cpp
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#include "Core/Thread.h"
#include "Core/Allocator.h"
#include "Core/Assert.h"

namespace Threading
{

struct NativeThreadParams
{
	ThreadFunctionType	   function;
	NativeThreadParamsType params;

	static void Create(NativeThreadParams** thread_params, const Thread::Ci& ci RESULT_ARG_OPT);
	static void Destroy(NativeThreadParams** thread_params RESULT_ARG_OPT);
};

void NativeThreadParams::Create(NativeThreadParams** thread_params, const Thread::Ci& ci RESULT_ARG)
{
	if (!thread_params)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
	if (*thread_params)
	{
		RESULT_ERROR(eResultErrorPtrIsNotNull);
	}
	*thread_params =
		static_cast<NativeThreadParams*>(Allocators::Default(NAME("thread")).allocate(sizeof(NativeThreadParams)));
	(*thread_params)->function = ci.function;

	if (ci.params && ci.params_size)
	{
		void* l_params = Allocators::Default(NAME("thread")).allocate(ci.params_size);
		memcpy(l_params, ci.params, ci.params_size);
		(*thread_params)->params = l_params;
	}

	RESULT_OK();
}

void NativeThreadParams::Destroy(NativeThreadParams** thread_params RESULT_ARG)
{
	if (!(thread_params && *thread_params))
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
	if ((*thread_params)->params)
	{
		Allocators::Default(NAME("thread")).deallocate((*thread_params)->params, 0);
		(*thread_params)->params = nullptr;
	}
	RESULT_OK();
}

#if PLATFORM_WINDOWS
static DWORD NativeThreadFunctionCall(void* params)
{
	const auto l_params = static_cast<NativeThreadParams*>(params);
	return l_params->function(l_params->params);
}

static DWORD GetCreationFlags(const NativeThreadCiFlags::Type flags)
{
	return ((flags & NativeThreadCiFlags::eCreateSuspended) ? CREATE_SUSPENDED : 0);
}
#endif

Thread::Thread(RESULT_ARG_SINGLE)
{
}

Thread::Thread(const Ci& ci RESULT_ARG)
{
	Create(eastl::move(ci));
}

Thread::Thread(const ThreadFunctionType function RESULT_ARG)
{
	Create(Ci{function, nullptr, 0ull, NativeThreadCiFlags::eNone});
}

Thread::~Thread()
{
	Destroy();
}

void Thread::Create(const Ci& ci RESULT_ARG)
{
	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
	if (handle_.ptr || handle_.params)
	{
		RESULT_ERROR(eResultErrorPtrIsNotNull);
	}

	NativeThreadParams* l_thread_params{};
	RESULT_ENSURE_CALL_NL(NativeThreadParams::Create(&l_thread_params, ci));

#if PLATFORM_WINDOWS
	handle_.ptr =
		CreateThread(nullptr, 0, NativeThreadFunctionCall, l_thread_params, GetCreationFlags(ci.flags), nullptr);
#else
#error Not supported yet.
#endif
	handle_.params = l_thread_params;

	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorThreadCreateFailed);
	}

	RESULT_OK();
}

void Thread::Sleep(const Uint32 ms RESULT_ARG) const
{
	if (WaitForSingleObject(handle_.ptr, ms) != WAIT_TIMEOUT)
	{
		RESULT_ERROR(eResultErrorThreadSleepFailed);
	}
	RESULT_OK();
}

void Thread::Suspend(RESULT_ARG_SINGLE) const
{
	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
#if PLATFORM_WINDOWS

	if (SuspendThread(handle_.ptr) == static_cast<DWORD>(-1))
	{
		RESULT_ERROR(eResultErrorThreadSuspendFailed);
	}

	RESULT_OK();

#else
#error Not supported yet.
#endif
}

void Thread::Resume(RESULT_ARG_SINGLE) const
{
	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}

#if PLATFORM_WINDOWS

	if (ResumeThread(handle_.ptr) == static_cast<DWORD>(-1))
	{
		RESULT_ERROR(eResultErrorThreadResumeFailed);
	}

	RESULT_OK();

#else
#error Not supported yet.
#endif
}

void Thread::Destroy(RESULT_ARG_SINGLE)
{
	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}

#if PLATFORM_WINDOWS

	if (CloseHandle(handle_.ptr) == FALSE)
	{
		RESULT_ERROR(eResultErrorThreadDestroyFailed);
	}

#else
#error Not supported yet.
#endif

	RESULT_ENSURE_CALL_NL(NativeThreadParams::Destroy(reinterpret_cast<NativeThreadParams**>(&handle_.params)));

	handle_.ptr	   = nullptr;
	handle_.params = nullptr;

	RESULT_OK();
}

Mutex::Mutex(RESULT_ARG_SINGLE)
{
}

Mutex::Mutex(const Ci& ci RESULT_ARG)
{
	Create(ci);
}

Mutex::Mutex(const char* name RESULT_ARG)
{
	Create(Ci{name});
}

Mutex::~Mutex()
{
	Destroy();
}

void Mutex::Create(const Ci& ci RESULT_ARG)
{
	Destroy();

#if PLATFORM_WINDOWS
	handle_.ptr = CreateMutexA(nullptr, 0, ci.name);
#else
#error Not supported yet.
#endif

	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorMutexCreateFailed);
	}

	RESULT_OK();
}

void Mutex::Lock(RESULT_ARG_SINGLE) const
{
	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
#if PLATFORM_WINDOWS
	if (WaitForSingleObject(handle_.ptr, INFINITE) != WAIT_OBJECT_0)
	{
		RESULT_ERROR(eResultErrorMutexLockFailed);
	}
#else
#error Not supported yet.
#endif
	RESULT_OK();
}

void Mutex::Unlock(RESULT_ARG_SINGLE) const
{
	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
#if PLATFORM_WINDOWS

	if (ReleaseMutex(handle_.ptr) == FALSE)
	{
		RESULT_ERROR(eResultErrorMutexUnlockFailed);
	}

	RESULT_OK();
#else
#error Not supported yet.
#endif
}

void Mutex::Destroy(RESULT_ARG_SINGLE)
{
	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
#if PLATFORM_WINDOWS

	if (CloseHandle(handle_.ptr) == FALSE)
	{
		RESULT_ERROR(eResultErrorMutexDestroyFailed);
	}

	handle_.ptr = nullptr;
	RESULT_OK();
#else
#error Not supported yet.
#endif
}

} // namespace Threading
