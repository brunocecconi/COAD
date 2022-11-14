
#include "Core/Thread.h"
#include "Core/Allocator.h"
#include "Core/Assert.h"

namespace Threading
{

struct NativeThreadParams
{
	ThreadFunctionType function;
	NativeThreadParamsType params;

	static void create(NativeThreadParams** thread_params, const Thread::Ci& ci);
	static void destroy(NativeThreadParams** thread_params);
};

void NativeThreadParams::create(NativeThreadParams** thread_params, const Thread::Ci& ci)
{
	if (!thread_params)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
	if (*thread_params)
	{
		RESULT_ERROR(eResultErrorPtrIsNotNull);
	}
	*thread_params = static_cast<NativeThreadParams*>(Allocators::Default(NAME("thread")).allocate(sizeof(NativeThreadParams)));
	(*thread_params)->function = ci.function;

	if (ci.params && ci.params_size)
	{
		void* l_params = Allocators::Default(NAME("thread")).allocate(ci.params_size);
		memcpy(l_params, ci.params, ci.params_size);
		(*thread_params)->params = l_params;
	}

	RESULT_OK();
}

void NativeThreadParams::destroy(NativeThreadParams** thread_params)
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
static DWORD nativeThreadFunctionCall(void* params)
{
	const auto l_params = static_cast<NativeThreadParams*>(params);
	return static_cast<DWORD>(l_params->function(l_params->params));
}

static DWORD getCreationFlags(const NativeThreadCiFlags::Type flags)
{
	return ((flags & NativeThreadCiFlags::eCreateSuspended) ? CREATE_SUSPENDED : 0);
}
#endif

Thread::Thread()
{
}

Thread::Thread(const Ci& ci)
{
	Create(eastl::move(ci));
}

Thread::Thread(const ThreadFunctionType function)
{
	Create(Ci{function, nullptr, 0ull, NativeThreadCiFlags::eNone});
}

Thread::~Thread()
{
	Destroy();
}

void Thread::Create(const Ci& ci)
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
	RESULT_ENSURE_CALL_NL(NativeThreadParams::create(&l_thread_params, ci));

#if PLATFORM_WINDOWS
	handle_.ptr = CreateThread(nullptr, 0, nativeThreadFunctionCall, l_thread_params, getCreationFlags(ci.flags), nullptr);
#else
#error Not supported yet.
#endif
	handle_.params = l_thread_params;

	if(!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorThreadCreateFailed);
	}

	RESULT_OK();
}

void Thread::Sleep(const u32 ms) const
{
	if(WaitForSingleObject(handle_.ptr, ms) != WAIT_TIMEOUT)
	{
		RESULT_ERROR(eResultErrorThreadSleepFailed);
	}
	RESULT_OK();
}

void Thread::Suspend() const
{
	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
#if PLATFORM_WINDOWS

	if(SuspendThread(handle_.ptr) == static_cast<DWORD>(-1))
	{
		RESULT_ERROR(eResultErrorThreadSuspendFailed);
	}

	RESULT_OK();

#else
#error Not supported yet.
#endif
}

void Thread::Resume() const
{
	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}

#if PLATFORM_WINDOWS

	if(ResumeThread(handle_.ptr) == static_cast<DWORD>(-1))
	{
		RESULT_ERROR(eResultErrorThreadResumeFailed);
	}

	RESULT_OK();

#else
#error Not supported yet.
#endif
}

void Thread::Destroy()
{
	if (!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}

#if PLATFORM_WINDOWS

	if(CloseHandle(handle_.ptr) == FALSE)
	{
		RESULT_ERROR(eResultErrorThreadDestroyFailed);
	}

#else
#error Not supported yet.
#endif

	RESULT_ENSURE_CALL_NL(NativeThreadParams::destroy(reinterpret_cast<NativeThreadParams**>(&handle_.params)));

	handle_.ptr = nullptr;
	handle_.params = nullptr;

	RESULT_OK();
}

Mutex::Mutex()
{
}

Mutex::Mutex(const Ci& ci)
{
	create(ci);
}

Mutex::Mutex(const char* name)
{
	create(Ci{name});
}

Mutex::~Mutex()
{
	destroy();
}

void Mutex::create(const Ci& ci)
{
	destroy();

#if PLATFORM_WINDOWS
	handle_.ptr = CreateMutexA(nullptr, 0, ci.name);	
#else
#error Not supported yet.
#endif

	if(!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorMutexCreateFailed);
	}

	RESULT_OK();
}

void Mutex::lock() const
{
	if(!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
#if PLATFORM_WINDOWS
	if(WaitForSingleObject(handle_.ptr, INFINITE) != WAIT_OBJECT_0 )
	{
		RESULT_ERROR(eResultErrorMutexLockFailed);
	}
#else
#error Not supported yet.
#endif
	RESULT_OK();
}

void Mutex::unlock() const
{
	if(!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
#if PLATFORM_WINDOWS

	if(ReleaseMutex(handle_.ptr) == FALSE)
	{
		RESULT_ERROR(eResultErrorMutexUnlockFailed);
	}

	RESULT_OK();
#else
#error Not supported yet.
#endif
}

void Mutex::destroy()
{
	if(!handle_.ptr)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
#if PLATFORM_WINDOWS

	if(CloseHandle(handle_.ptr) == FALSE)
	{
		RESULT_ERROR(eResultErrorMutexDestroyFailed);
	}

	handle_.ptr = nullptr;
	RESULT_OK();
#else
#error Not supported yet.
#endif
}

}
