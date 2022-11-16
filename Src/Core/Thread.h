
#ifndef CORE_THREAD_H
#define CORE_THREAD_H

#include "Core/Common.h"

namespace Threading
{

#if PLATFORM_WINDOWS
using NativeThreadParamsType = void*;
using NativeThreadHandleType = HANDLE;
using NativeMutexHandleType = HANDLE;
#endif

using ThreadFunctionType = u32(*)(NativeThreadParamsType);

namespace NativeThreadCiFlags
{
enum Type
{
	eNone,
	eCreateSuspended
};
}

class Thread
{
	CLASS_BODY_NON_COPYABLE(Thread)

public:
	/**
	 * @brief Thread handle.
	 * 
	*/
	struct Handle
	{
		NativeThreadHandleType ptr;
		NativeThreadParamsType params;
	};

	/**
	 * @brief Structure to thread create info.
	 * 
	*/
	struct Ci
	{
		ThreadFunctionType function;
		NativeThreadParamsType params;
		u64 params_size;
		NativeThreadCiFlags::Type flags;
	};

public:
	Thread(RESULT_ARG_SINGLE);
	EXPLICIT Thread(const Ci& ci RESULT_ARG_OPT);
	EXPLICIT Thread(ThreadFunctionType function RESULT_ARG_OPT);

	template < typename TypeArgs >
	EXPLICIT Thread(ThreadFunctionType function, TypeArgs& args RESULT_ARG_OPT);

	~Thread();

public:
	MAYBEUNUSED void Create(const Ci& ci RESULT_ARG_OPT);
	MAYBEUNUSED void Sleep(u32 ms RESULT_ARG_OPT) const;
	MAYBEUNUSED void Suspend(RESULT_ARG_SINGLE_OPT) const;
	MAYBEUNUSED void Resume(RESULT_ARG_SINGLE_OPT) const;
	MAYBEUNUSED void Destroy(RESULT_ARG_SINGLE_OPT);

private:
	Handle handle_{};
};

template <typename TypeArgs>
Thread::Thread(const ThreadFunctionType function, TypeArgs& args RESULT_ARG)
{
	Create(Ci
	{
		function,
		&args,
		sizeof(TypeArgs)
	});
}

class Mutex
{
	CLASS_BODY_NON_COPYABLE(Mutex)

public:
	struct Handle
	{
		NativeMutexHandleType ptr;
		const char* name;
	};

	struct Ci
	{
		const char* name;
	};

public:
	Mutex(RESULT_ARG_SINGLE);
	EXPLICIT Mutex(const Ci& ci RESULT_ARG_OPT);
	EXPLICIT Mutex(const char* name RESULT_ARG_OPT);
	~Mutex();

public:
	MAYBEUNUSED void Create(const Ci& ci RESULT_ARG_OPT);
	MAYBEUNUSED void Lock(RESULT_ARG_SINGLE_OPT) const;
	MAYBEUNUSED void Unlock(RESULT_ARG_SINGLE_OPT) const;
	MAYBEUNUSED void Destroy(RESULT_ARG_SINGLE_OPT);

private:
	Handle handle_{};
};

#define thread_lambda	[](native_thread_params_t)

}

CLASS_VALIDATION(Threading::Thread)
CLASS_VALIDATION(Threading::Mutex)

#endif
