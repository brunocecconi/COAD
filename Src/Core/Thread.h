
/** \file Thread.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_THREAD_H
#define CORE_THREAD_H

#include "Core/Common.h"

namespace Threading
{

#if PLATFORM_WINDOWS
using NativeThreadParamsType = void*;
using NativeThreadHandleType = HANDLE;
using NativeMutexHandleType	 = HANDLE;
#endif

using ThreadFunctionType = Uint32 (*)(NativeThreadParamsType);

namespace NativeThreadCiFlags
{
enum Type
{
	eNone,
	eCreateSuspended
};
} // namespace NativeThreadCiFlags

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
		ThreadFunctionType		  function;
		NativeThreadParamsType	  params;
		Uint64					  params_size;
		NativeThreadCiFlags::Type flags;
	};

public:
	Thread(RESULT_PARAM_DEFINE);
	EXPLICIT Thread(const Ci& ci, RESULT_PARAM_DEFINE);
	EXPLICIT Thread(ThreadFunctionType function, RESULT_PARAM_DEFINE);

	template<typename TypeArgs>
	EXPLICIT Thread(ThreadFunctionType function, TypeArgs& args, RESULT_PARAM_DEFINE);

	~Thread();

public:
	MAYBEUNUSED void Create(const Ci& ci, RESULT_PARAM_DEFINE);
	MAYBEUNUSED void Sleep(Uint32 ms, RESULT_PARAM_DEFINE) const;
	MAYBEUNUSED void Suspend(RESULT_PARAM_DEFINE) const;
	MAYBEUNUSED void Resume(RESULT_PARAM_DEFINE) const;
	MAYBEUNUSED void Destroy(RESULT_PARAM_DEFINE);

private:
	Handle handle_{};
};

template<typename TypeArgs>
Thread::Thread(const ThreadFunctionType function, TypeArgs& args, RESULT_PARAM_IMPL)
{
	Create(Ci{function, &args, sizeof(TypeArgs)});
}

class Mutex
{
	CLASS_BODY_NON_COPYABLE(Mutex)

public:
	struct Handle
	{
		NativeMutexHandleType ptr;
		const char*			  name;
	};

	struct Ci
	{
		const char* name;
	};

public:
	Mutex(RESULT_PARAM_DEFINE);
	EXPLICIT Mutex(const Ci& ci, RESULT_PARAM_DEFINE);
	EXPLICIT Mutex(const char* name, RESULT_PARAM_DEFINE);
	~Mutex();

public:
	MAYBEUNUSED void Create(const Ci& ci, RESULT_PARAM_DEFINE);
	MAYBEUNUSED void Lock(RESULT_PARAM_DEFINE) const;
	MAYBEUNUSED void Unlock(RESULT_PARAM_DEFINE) const;
	MAYBEUNUSED void Destroy(RESULT_PARAM_DEFINE);

private:
	Handle handle_{};
};

#define thread_lambda [](native_thread_params_t)

} // namespace Threading

CLASS_VALIDATION(Threading::Thread);
CLASS_VALIDATION(Threading::Mutex);

#endif
