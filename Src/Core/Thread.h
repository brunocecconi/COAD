
#ifndef CORE_THREAD_H
#define CORE_THREAD_H

#include "core/common.h"

namespace Threading
{

#if PLATFORM_WINDOWS
using NativeThreadParamsType = void*;
using NativeThreadHandleType = HANDLE;
using NativeMutexHandleType = HANDLE;
#endif

using ThreadFunctionType = Result(*)(NativeThreadParamsType);

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
	Thread();
	EXPLICIT Thread(const Ci& ci);
	EXPLICIT Thread(ThreadFunctionType function);

	template < typename TypeArgs >
	EXPLICIT Thread(ThreadFunctionType function, TypeArgs& args);

	~Thread();

public:
	MAYBEUNUSED void Create(const Ci& ci);
	MAYBEUNUSED void Sleep(u32 ms) const;
	MAYBEUNUSED void Suspend() const;
	MAYBEUNUSED void Resume() const;
	MAYBEUNUSED void Destroy();

private:
	Handle handle_{};
};

template <typename TypeArgs>
Thread::Thread(const ThreadFunctionType function, TypeArgs& args)
{
	create(Ci
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
	Mutex();
	EXPLICIT Mutex(const Ci& ci);
	EXPLICIT Mutex(const char* name);
	~Mutex();

public:
	MAYBEUNUSED void create(const Ci& ci);
	MAYBEUNUSED void lock() const;
	MAYBEUNUSED void unlock() const;
	MAYBEUNUSED void destroy();

private:
	Handle handle_{};
};

#define thread_lambda	[](native_thread_params_t)

}

CLASS_VALIDATION(Threading::Thread)
CLASS_VALIDATION(Threading::Mutex)

#endif
