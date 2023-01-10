
#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H

#include "Core/Common.h"
#include "Core/Thread.h"

#include <EASTL/unique_ptr.h>

template<typename T>
struct ManagerWait
{
	EResult* result;
	EXPLICIT ManagerWait(RESULT_PARAM_DEFINE);
	~ManagerWait();
};

struct Test
{
	int	  a;
	float b;
};

template<typename T>
class ManagerNoThread
{
protected:
	void Initialize(RESULT_PARAM_DEFINE);
	void Run(RESULT_PARAM_DEFINE);
	void Finalize(RESULT_PARAM_DEFINE);

public:
	NODISCARD bool		   IsInitialized() const;
	NODISCARD bool		   IsRunning() const;
	NODISCARD const Mutex& GetMutex() const;
	NODISCARD static T&	   Instance();
	NODISCARD bool		   IsInThread() const;

protected:
	bool						mInitialized{};
	bool						mRunning{};
	Mutex						mMutex{};
	uint32_t					mThreadId{};
	static eastl::unique_ptr<T> mInstance;
};

template<typename T>
ManagerWait<T>::ManagerWait(RESULT_PARAM_IMPL) : result{RESULT_ARG_PASS}
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(T::Instance().GetMutex().Lock(RESULT_ARG_PASS));
	RESULT_OK();
}

template<typename T>
ManagerWait<T>::~ManagerWait()
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(T::Instance().GetMutex().Unlock(RESULT_ARG_PASS));
	RESULT_OK();
}

template<typename T>
void ManagerNoThread<T>::Initialize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(!mInitialized, AlreadyInitialized);
	RESULT_ENSURE_CALL(mMutex.Create({}, RESULT_ARG_PASS));
	mInitialized = true;
	mThreadId	 = GetCurrentThreadId();
	RESULT_OK();
}

template<typename T>
void ManagerNoThread<T>::Run(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(mInitialized, NotInitialized);
	mRunning = true;
	while (mRunning)
	{
		mInstance->RunInternal(RESULT_ARG_PASS);
	}
	RESULT_OK();
}

template<typename T>
void ManagerNoThread<T>::Finalize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(mInitialized, NotInitialized);
	mInitialized = false;
	mRunning	 = false;
	RESULT_OK();
}

template<typename T>
bool ManagerNoThread<T>::IsInitialized() const
{
	return mInitialized;
}

template<typename T>
bool ManagerNoThread<T>::IsRunning() const
{
	return mRunning;
}

template<typename T>
const Mutex& ManagerNoThread<T>::GetMutex() const
{
	return mMutex;
}

template<typename T>
T& ManagerNoThread<T>::Instance()
{
	if (!mInstance)
	{
		mInstance.reset(new (Allocators::Default{DEBUG_NAME_VAL("Manager")}.allocate(sizeof(T))) T{});
	}
	return *mInstance;
}

template<typename T>
bool ManagerNoThread<T>::IsInThread() const
{
	return mThreadId == GetCurrentThreadId();
}

template<typename T>
class ManagerThread
{
protected:
	void Initialize(RESULT_PARAM_DEFINE);
	void Run(RESULT_PARAM_DEFINE);
	void Finalize(RESULT_PARAM_DEFINE);
	void WaitThreadFinish(RESULT_PARAM_DEFINE) const;

private:
	static uint32_t ThreadRun(thread_native_params_t Args);

public:
	NODISCARD bool		   IsInitialized() const;
	NODISCARD bool		   IsRunning() const;
	NODISCARD const Mutex& GetMutex() const;
	NODISCARD static T&	   Instance();
	NODISCARD bool		   IsInThread() const;

protected:
	bool						mInitialized{};
	bool						mRunning{};
	Thread						mThread{};
	Mutex						mMutex{};
	static eastl::unique_ptr<T> mInstance;
};

template<typename T>
void ManagerThread<T>::Initialize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(!mInitialized, AlreadyInitialized);

	// Create info struct.
	Thread::CreateInfo l_create_info{};
	l_create_info.Function		= ThreadRun;
	l_create_info.Flags			= ThreadNativeCiFlags::eCreateSuspended;
	l_create_info.DestroyOnDtor = false;

	RESULT_ENSURE_CALL(mThread.Create(l_create_info, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mMutex.Create({}, RESULT_ARG_PASS));
	mInitialized = true;
	RESULT_OK();
}

template<typename T>
void ManagerThread<T>::Run(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(mInitialized, NotInitialized);
	mRunning = true;
	RESULT_ENSURE_CALL(mThread.Resume(RESULT_ARG_PASS));
	RESULT_OK();
}

template<typename T>
void ManagerThread<T>::Finalize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(mInitialized, NotInitialized);
	mInitialized = false;
	mRunning	 = false;
	RESULT_OK();
}

template<typename T>
void ManagerThread<T>::WaitThreadFinish(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(mThread.Wait());
	RESULT_OK();
}

template<typename T>
uint32_t ManagerThread<T>::ThreadRun(thread_native_params_t Args)
{
	EResult result = Ok;

	while (mInstance->mRunning)
	{
		RESULT_VALUE_ENSURE_CALL(mInstance->RunInternal(RESULT_ARG_VALUE_PASS), EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

template<typename T>
bool ManagerThread<T>::IsInitialized() const
{
	return mInitialized;
}

template<typename T>
bool ManagerThread<T>::IsRunning() const
{
	return mRunning;
}

template<typename T>
const Mutex& ManagerThread<T>::GetMutex() const
{
	return mMutex;
}

template<typename T>
T& ManagerThread<T>::Instance()
{
	if (!mInstance)
	{
		mInstance.reset(new (Allocators::Default{DEBUG_NAME_VAL("Manager")}.allocate(sizeof(T))) T{});
	}
	return *mInstance;
}

template<typename T>
bool ManagerThread<T>::IsInThread() const
{
	return GetThreadId(mThread.GetHandle().Ptr) == GetCurrentThreadId();
}

#define MANAGER_IMPL(TYPE)                                                                                             \
	eastl::unique_ptr<TYPE> ManagerThread<TYPE>::mInstance
#define MANAGER_NO_THREAD_IMPL(TYPE)                                                                                   \
	eastl::unique_ptr<TYPE> ManagerNoThread<TYPE>::mInstance

#endif
