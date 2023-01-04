
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

#if PLATFORM_WINDOWS
using thread_native_params_t = void*;
using thread_native_handle_t = HANDLE;
using mutex_native_handle_t	 = HANDLE;
#endif

using thread_function_t = uint32_t (*)(thread_native_params_t);

namespace ThreadNativeCiFlags
{
enum Type
{
	eNone,
	eCreateSuspended,
	eAutoDestroy
};
} // namespace ThreadNativeCiFlags

/**
 * @brief Thread class.
 *
 */
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
		thread_native_handle_t Ptr;
		thread_native_params_t Params;
	};

	/**
	 * @brief Structure to thread create info.
	 *
	 */
	struct CreateInfo
	{
		thread_function_t		  Function{};
		thread_native_params_t	  Params{};
		uint64_t				  ParamsSize{};
		ThreadNativeCiFlags::Type Flags{};
		bool					  DestroyOnDtor{true};
	};

public:
	Thread(RESULT_PARAM_DEFINE);
	EXPLICIT Thread(const CreateInfo& CreateInfo, RESULT_PARAM_DEFINE);
	~Thread();

public:
	void			 Create(const CreateInfo& CreateInfo, RESULT_PARAM_DEFINE);
	void			 Sleep(uint32_t Milliseconds, RESULT_PARAM_DEFINE) const;
	static void		 SleepCurrent(uint32_t Milliseconds, RESULT_PARAM_DEFINE);
	void			 Suspend(RESULT_PARAM_DEFINE) const;
	void			 Resume(RESULT_PARAM_DEFINE) const;
	void			 Destroy(RESULT_PARAM_DEFINE);
	void			 SetAffinity(uint64_t Index, RESULT_PARAM_DEFINE) const;
	static void		 SetAffinity(const Handle& Handle, uint64_t Index, RESULT_PARAM_DEFINE);
	void			 Wait(RESULT_PARAM_DEFINE) const;
	void			 Wait(uint32_t Milliseconds, RESULT_PARAM_DEFINE) const;
	NODISCARD Handle GetHandle() const;

public:
	static Handle HandleCurrent();

private:
	Handle mHandle{};
	bool   mDestroyOnDtor{};
};

NODISCARD bool IsThread(const Thread& Thread);

/**
 * @brief Mutex class.
 *
 */
class Mutex
{
	CLASS_BODY_NON_COPYABLE(Mutex)

public:
	struct Scope
	{
		Mutex*	 Mtx;
		EResult* result;
		EXPLICIT Scope(Mutex* Mutex, RESULT_PARAM_DEFINE);
		~Scope();
	};

	template<typename TOwner>
	struct ScopeFromOwner
	{
		TOwner*	 Owner;
		EResult* result;
		EXPLICIT ScopeFromOwner(TOwner* Owner, RESULT_PARAM_DEFINE);
		~ScopeFromOwner();
	};

	struct Handle
	{
		mutex_native_handle_t Ptr;
		const char*			  Name;
	};

	struct CreateInfo
	{
		const char* Name;
	};

public:
	EXPLICIT Mutex(RESULT_PARAM_DEFINE);
	EXPLICIT Mutex(const CreateInfo& CreateInfo, RESULT_PARAM_DEFINE);
	EXPLICIT Mutex(const char* Name, RESULT_PARAM_DEFINE);
	~Mutex();

public:
	MAYBEUNUSED void Create(const CreateInfo& CreateInfo, RESULT_PARAM_DEFINE);
	MAYBEUNUSED void Lock(RESULT_PARAM_DEFINE) const;
	MAYBEUNUSED void Unlock(RESULT_PARAM_DEFINE) const;
	MAYBEUNUSED void Destroy(RESULT_PARAM_DEFINE);

private:
	Handle handle_{};
};

template<typename OwnerClass>
Mutex::ScopeFromOwner<OwnerClass>::ScopeFromOwner(OwnerClass* Owner, RESULT_PARAM_IMPL)
	: Owner{Owner}, result{RESULT_ARG_PASS}
{
	RESULT_ENSURE_LAST();
	Owner->mMutex.Lock();
	RESULT_OK();
}

template<typename OwnerClass>
Mutex::ScopeFromOwner<OwnerClass>::~ScopeFromOwner()
{
	RESULT_ENSURE_LAST();
	Owner->mMutex.Unlock();
	RESULT_OK();
}

#define thread_lambda [](native_thread_params_t)

CLASS_VALIDATION(Thread);
CLASS_VALIDATION(Mutex);

#endif
