
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

struct ThreadNativeParams
{
	thread_function_t	   function;
	thread_native_params_t params;

	static void Create(ThreadNativeParams** ThreadParams, const Thread::CreateInfo& CreateInfo, RESULT_PARAM_DEFINE);
	static void Destroy(ThreadNativeParams** ThreadParams, RESULT_PARAM_DEFINE);
};

void ThreadNativeParams::Create(ThreadNativeParams** ThreadParams, const Thread::CreateInfo& CreateInfo,
								RESULT_PARAM_IMPL)
{
	if (!ThreadParams)
	{
		RESULT_ERROR(NullPtr);
	}
	if (*ThreadParams)
	{
		RESULT_ERROR(PtrIsNotNull);
	}
	*ThreadParams = static_cast<ThreadNativeParams*>(
		Allocators::default_t(DEBUG_NAME("Thread")).allocate(sizeof(ThreadNativeParams)));
	(*ThreadParams)->function = CreateInfo.Function;

	if (CreateInfo.Params && CreateInfo.ParamsSize)
	{
		void* lParams = Allocators::default_t(DEBUG_NAME("Thread")).allocate(CreateInfo.ParamsSize);
		memcpy(lParams, CreateInfo.Params, CreateInfo.ParamsSize);
		(*ThreadParams)->params = lParams;
	}

	RESULT_OK();
}

void ThreadNativeParams::Destroy(ThreadNativeParams** ThreadParams, RESULT_PARAM_IMPL)
{
	if (!(ThreadParams && *ThreadParams))
	{
		RESULT_ERROR(NullPtr);
	}
	if ((*ThreadParams)->params)
	{
		Allocators::default_t(DEBUG_NAME("Thread")).deallocate((*ThreadParams)->params, 0);
		(*ThreadParams)->params = nullptr;
	}
	RESULT_OK();
}

#if PLATFORM_WINDOWS
static DWORD NativeThreadFunctionCall(void* Params)
{
	const auto l_params = static_cast<ThreadNativeParams*>(Params);
	return l_params->function(l_params->params);
}

static DWORD GetCreationFlags(const ThreadNativeCiFlags::Type flags)
{
	return ((flags & ThreadNativeCiFlags::eCreateSuspended) ? CREATE_SUSPENDED : 0);
}
#endif

Thread::Thread(RESULT_PARAM_IMPL)
{
	RESULT_UNUSED();
}

Thread::Thread(const CreateInfo& CreateInfo, RESULT_PARAM_IMPL) : mDestroyOnDtor{CreateInfo.DestroyOnDtor}
{
	Create(eastl::move(CreateInfo));
	RESULT_UNUSED();
}

Thread::~Thread()
{
	if (mDestroyOnDtor)
	{
		Destroy();
	}
}

void Thread::Create(const CreateInfo& CreateInfo, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	if (mHandle.Ptr || mHandle.Params)
	{
		RESULT_ERROR(PtrIsNotNull);
	}

	ThreadNativeParams* lThreadParams{};
	RESULT_ENSURE_CALL(ThreadNativeParams::Create(&lThreadParams, CreateInfo));

#if PLATFORM_WINDOWS
	mHandle.Ptr =
		CreateThread(nullptr, 0, NativeThreadFunctionCall, lThreadParams, GetCreationFlags(CreateInfo.Flags), nullptr);
#else
#error Not supported yet.
#endif
	mHandle.Params = lThreadParams;

	if (!mHandle.Ptr)
	{
		RESULT_ERROR(ThreadCreateFailed);
	}

	RESULT_OK();
}

void Thread::Sleep(const uint32_t Milliseconds, RESULT_PARAM_IMPL) const
{
	if (WaitForSingleObject(mHandle.Ptr, Milliseconds) != WAIT_TIMEOUT)
	{
		RESULT_ERROR(ThreadSleepFailed);
	}
	RESULT_OK();
}

void Thread::SleepCurrent(const uint32_t Milliseconds, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	// RESULT_CONDITION_ENSURE(Milliseconds > 0, ZeroTime);
	::Sleep(Milliseconds);
	RESULT_OK();
}

void Thread::Suspend(RESULT_PARAM_IMPL) const
{
	if (!mHandle.Ptr)
	{
		RESULT_ERROR(NullPtr);
	}
#if PLATFORM_WINDOWS

	if (SuspendThread(mHandle.Ptr) == static_cast<DWORD>(-1))
	{
		RESULT_ERROR(ThreadSuspendFailed);
	}

	RESULT_OK();

#else
#error Not supported yet.
#endif
}

void Thread::Resume(RESULT_PARAM_IMPL) const
{
	if (!mHandle.Ptr)
	{
		RESULT_ERROR(NullPtr);
	}

#if PLATFORM_WINDOWS

	if (ResumeThread(mHandle.Ptr) == static_cast<DWORD>(-1))
	{
		RESULT_ERROR(ThreadResumeFailed);
	}

	RESULT_OK();

#else
#error Not supported yet.
#endif
}

void Thread::Destroy(RESULT_PARAM_IMPL)
{
	if (!mHandle.Ptr)
	{
		RESULT_ERROR(NullPtr);
	}

#if PLATFORM_WINDOWS

	if (CloseHandle(mHandle.Ptr) == FALSE)
	{
		RESULT_ERROR(ThreadDestroyFailed);
	}

#else
#error Not supported yet.
#endif

	RESULT_ENSURE_CALL(ThreadNativeParams::Destroy(reinterpret_cast<ThreadNativeParams**>(&mHandle.Params)));

	mHandle.Ptr	   = nullptr;
	mHandle.Params = nullptr;

	RESULT_OK();
}

void Thread::SetAffinity(const uint64_t Index, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(mHandle.Ptr, NullPtr);
#if PLATFORM_WINDOWS
	RESULT_CONDITION_ENSURE(SetThreadAffinityMask(mHandle.Ptr, 1ull << Index) != ERROR_INVALID_PARAMETER,
							ThreadAffinityFailed);
#else
#error Not supported yet.
#endif
	RESULT_OK();
}

void Thread::SetAffinity(const Handle& Handle, const uint64_t Index, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(Handle.Ptr, NullPtr);
#if PLATFORM_WINDOWS
	RESULT_CONDITION_ENSURE(SetThreadAffinityMask(Handle.Ptr, 1ull << Index) != ERROR_INVALID_PARAMETER,
							ThreadAffinityFailed);
#else
#error Not supported yet.
#endif
	RESULT_OK();
}

void Thread::Wait(RESULT_PARAM_IMPL) const
{
	Wait(eastl::numeric_limits<uint32_t>::max(), RESULT_ARG_PASS);
}

void Thread::Wait(const uint32_t Milliseconds, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(mHandle.Ptr, NullPtr);
	RESULT_CONDITION_ENSURE(Milliseconds > 0, ZeroTime);
#if PLATFORM_WINDOWS
	RESULT_CONDITION_ENSURE(WaitForSingleObject(mHandle.Ptr, Milliseconds) != WAIT_FAILED, ThreadWaitFailed);
#else
#error Not supported yet.
#endif
	RESULT_OK();
}

Thread::Handle Thread::GetHandle() const
{
	return mHandle;
}

Thread::Handle Thread::HandleCurrent()
{
	return {GetCurrentThread(), {}};
}

Mutex::Scope::Scope(Mutex* Mutex, RESULT_PARAM_IMPL) : Mtx{Mutex}, result{RESULT_ARG_PASS}
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(Mutex->Lock(RESULT_ARG_PASS));
	RESULT_OK();
}

Mutex::Scope::~Scope()
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(Mtx->Unlock(RESULT_ARG_PASS));
	RESULT_OK();
}

Mutex::Mutex(RESULT_PARAM_IMPL)
{
	RESULT_UNUSED();
}

Mutex::Mutex(const CreateInfo& CreateInfo, RESULT_PARAM_IMPL)
{
	Create(CreateInfo);
	RESULT_UNUSED();
}

Mutex::Mutex(const char* Name, RESULT_PARAM_IMPL)
{
	Create(CreateInfo{Name});
	RESULT_UNUSED();
}

Mutex::~Mutex()
{
	Destroy();
}

void Mutex::Create(const CreateInfo& CreateInfo, RESULT_PARAM_IMPL)
{
	Destroy();

#if PLATFORM_WINDOWS
	handle_.Ptr = CreateMutexA(nullptr, 0, CreateInfo.Name);
#else
#error Not supported yet.
#endif

	if (!handle_.Ptr)
	{
		RESULT_ERROR(MutexCreateFailed);
	}

	RESULT_OK();
}

void Mutex::Lock(RESULT_PARAM_IMPL) const
{
	RESULT_CONDITION_ENSURE(handle_.Ptr, NullPtr);
#if PLATFORM_WINDOWS
	RESULT_CONDITION_ENSURE(WaitForSingleObject(handle_.Ptr, INFINITE) == WAIT_OBJECT_0, MutexLockFailed);
#else
#error Not supported yet.
#endif
	RESULT_OK();
}

void Mutex::Unlock(RESULT_PARAM_IMPL) const
{
	RESULT_CONDITION_ENSURE(handle_.Ptr, NullPtr);
#if PLATFORM_WINDOWS
	RESULT_CONDITION_ENSURE(ReleaseMutex(handle_.Ptr) == TRUE, MutexUnlockFailed);
	RESULT_OK();
#else
#error Not supported yet.
#endif
}

void Mutex::Destroy(RESULT_PARAM_IMPL)
{
	RESULT_CONDITION_ENSURE_NOLOG(handle_.Ptr, NullPtr);
#if PLATFORM_WINDOWS
	RESULT_CONDITION_ENSURE(CloseHandle(handle_.Ptr) == TRUE, MutexUnlockFailed);
	handle_.Ptr = nullptr;
	RESULT_OK();
#else
#error Not supported yet.
#endif
}
