
/** @file Ptr.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_PTR_H
#define CORE_PTR_H

#include "Core/common.h"

#if defined(RELEASE)
#undef USE_SAFE_PTR
#define USE_SAFE_PTR 0
#endif

#if USE_SAFE_PTR

template<typename T, typename = void>
struct PtrTransferUnderlyng
{
	using Type = T;
};

template<typename T>
struct PtrTransferUnderlyng<T, eastl::void_t<typename T::UnderlyngType>>
{
	using Type = typename T::UnderlyngType;
};

template<typename T, bool IsPointer = true>
struct PtrTransfer
{
	using Type = eastl::remove_pointer_t<T>;
};

template<typename T>
struct PtrTransfer<T, false>
{
	using Type = typename PtrTransferUnderlyng<T>::Type;
};

template<typename T>
using PtrTransferType = typename PtrTransfer<T, eastl::is_pointer_v<T>>::Type;

template<typename T>
static PtrTransferType<T>* AssertIfPtrInvalid(PtrTransferType<T>* value, const char* function, const char* file,
											  const Uint32 line)
{
	if (!value)
	{
#ifndef NDEBUG
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD
			   "Invalid pointer declared at '%s':'%s':'%u'.\n" CONSOLE_COLOR_DEFAULT,
			   function, file, line);
#endif
		exit(-1);
		return value;
	}
	return value;
}

template<typename T>
static PtrTransferType<T>* WarnIfPtrInvalid(PtrTransferType<T>* value, const char* function, const char* file,
											const Uint32 line)
{
	if (!value)
	{
#ifndef NDEBUG
		printf(CONSOLE_COLOR_LIGHT_YELLOW CONSOLE_COLOR_BOLD
			   "Invalid pointer declared at '%s':'%s':'%u'.\n" CONSOLE_COLOR_DEFAULT,
			   function, file, line);
#endif
		return value;
	}
	return value;
}

#define PTRA(X) AssertIfPtrInvalid<decltype(X)>(X, (const char*)__FUNCSIG__, (const char*)__FILE__, (Uint32)__LINE__)
#define PTRW(X) WarnIfPtrInvalid<decltype(X)>(X, (const char*)__FUNCSIG__, (const char*)__FILE__, (Uint32)__LINE__)

#ifndef NDEBUG
#define PTR_ASSERT()                                                                                                   \
	if (!value_)                                                                                                       \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD                                                              \
			   "Invalid pointer declared at '%s':'%s':'%u'.\n" CONSOLE_COLOR_DEFAULT,                                  \
			   function_, file_, line_);                                                                               \
		exit(-1);                                                                                                      \
	}

#define PTR(VALUE)                                                                                                     \
	Ptr<PtrTransferType<decltype(VALUE)>>                                                                              \
	{                                                                                                                  \
		VALUE, (const char*)__FUNCSIG__, (const char*)__FILE__, (Uint32)__LINE__                                       \
	}
#define PTRC(X) PTR(X)
#else
#define PTR_ASSERT()
#define PTR(VALUE)                                                                                                     \
	Ptr<eastl::remove_pointer_t<decltype(VALUE)>>                                                                      \
	{                                                                                                                  \
		VALUE                                                                                                          \
	}
#define PTRC(X) X
#endif

template<typename T>
class Ptr
{
public:
	Ptr();
	Ptr(Ptr&&) NOEXCEPT;
	Ptr& operator=(const Ptr&);
	Ptr& operator=(Ptr&&) NOEXCEPT;
	~Ptr();

public:
	using UnderlyngType = T;

	Ptr(T* value, const char* function = nullptr, const char* file = nullptr, Uint32 line = 0u);

	template<typename Y,
			 typename = eastl::enable_if_t<!eastl::is_pointer_v<Y> ||
										   eastl::is_same_v<void, eastl::void_t<typename Y::UnderlyngType>>>>
	Ptr(Y& value, const char* function = nullptr, const char* file = nullptr, const Uint32 line = 0u);

	Ptr(const Ptr& value, const char* function = nullptr, const char* file = nullptr, Uint32 line = 0u);

	NODISCARD T& operator*();
	NODISCARD T* operator->();
	NODISCARD	 operator T*();
	NODISCARD T* get();
	NODISCARD T& operator*() const;
	NODISCARD T* operator->() const;
	NODISCARD	 operator bool() const;
	NODISCARD	 operator T*() const;
	NODISCARD T* get() const;

	NODISCARD bool operator==(const Ptr& value) const;
	NODISCARD bool operator!=(const Ptr& value) const;
	NODISCARD bool operator<(const Ptr& value) const;
	NODISCARD bool operator>(const Ptr& value) const;
	NODISCARD bool operator<=(const Ptr& value) const;
	NODISCARD bool operator>=(const Ptr& value) const;
	void		   operator++();
	void		   operator--();

private:
	T* value_{};
#ifndef NDEBUG
	char*  function_{};
	char*  file_{};
	Uint32 line_{};
#endif
};

template<typename T>
Ptr<T>::Ptr() = default;
template<typename T>
Ptr<T>::Ptr(Ptr&&) noexcept = default;
template<typename T>
Ptr<T>& Ptr<T>::operator=(const Ptr&) = default;
template<typename T>
Ptr<T>& Ptr<T>::operator=(Ptr&&) noexcept = default;
template<typename T>
Ptr<T>::~Ptr() = default;

template<typename T>
Ptr<T>::Ptr(T* value, const char* function, const char* file, const Uint32 line)
	: value_{value}
#ifndef NDEBUG
	  ,
	  function_{const_cast<char*>(function)}, file_{const_cast<char*>(file)}, line_{line}
#endif
{
#ifdef NDEBUG
	(void)function;
	(void)file;
	(void)line;
#endif
}

template<typename T>
template<typename Y, typename>
Ptr<T>::Ptr(Y& value, const char* function, const char* file, const Uint32 line)
	: value_{(T*)value.operator->()}
#ifndef NDEBUG
	  ,
	  function_{const_cast<char*>(function)}, file_{const_cast<char*>(file)}, line_{line}
#endif
{
}

template<typename T>
Ptr<T>::Ptr(const Ptr& value, const char* function, const char* file, const Uint32 line)
	: value_{value.value_}
#ifndef NDEBUG
	  ,
	  function_{const_cast<char*>(function)}, file_{const_cast<char*>(file)}, line_{line}
#endif
{
#ifdef NDEBUG
	(void)function;
	(void)file;
	(void)line;
#endif
}

template<typename T>
T& Ptr<T>::operator*()
{
	PTR_ASSERT();
	return *value_;
}

template<typename T>
T* Ptr<T>::operator->()
{
	PTR_ASSERT();
	return value_;
}

template<typename T>
Ptr<T>::operator T*()
{
	return value_;
}

template<typename T>
T* Ptr<T>::get()
{
	return value_;
}

template<typename T>
T& Ptr<T>::operator*() const
{
	PTR_ASSERT();
	return *value_;
}

template<typename T>
T* Ptr<T>::operator->() const
{
	PTR_ASSERT();
	return value_;
}

template<typename T>
Ptr<T>::operator bool() const
{
	return value_ != nullptr;
}

template<typename T>
Ptr<T>::operator T*() const
{
	return value_;
}

template<typename T>
T* Ptr<T>::get() const
{
	return value_;
}

template<typename T>
bool Ptr<T>::operator==(const Ptr& value) const
{
	return value_ == value.value_;
}

template<typename T>
bool Ptr<T>::operator!=(const Ptr& value) const
{
	return value_ != value.value_;
}

template<typename T>
bool Ptr<T>::operator<(const Ptr& value) const
{
	return value_ < value.value_;
}

template<typename T>
bool Ptr<T>::operator>(const Ptr& value) const
{
	return value_ > value.value_;
}

template<typename T>
bool Ptr<T>::operator<=(const Ptr& value) const
{
	return value_ <= value.value_;
}

template<typename T>
bool Ptr<T>::operator>=(const Ptr& value) const
{
	return value_ >= value.value_;
}

template<typename T>
void Ptr<T>::operator++()
{
	++value_;
}

template<typename T>
void Ptr<T>::operator--()
{
	--value_;
}

#else

#define PTRA(X) X
#define PTRW(X) X
#define PTRC(X) X
#define PTR(X)	X

template<typename T>
using Ptr = T*;

#endif

#endif
