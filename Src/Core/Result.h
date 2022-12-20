
/** @file Result.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_RESULT_H
#define CORE_RESULT_H

#include "Core/BaseTypes.h"

#if _MSC_VER
#pragma warning(disable : 4100)
#endif

#define RESULT Result
#define RESULT_VAR(NAME)                                                                                               \
	RESULT* NAME = nullptr;                                                                                            \
	(void)NAME

#define RESULT_VALUE_VAR(NAME)                                                                                               \
	RESULT NAME = eResultOk;                                                                                            \
	(void)NAME
#define RESULT_SET_VAR(NAME, VALUE) NAME = (Result)(VALUE)
#define RESULT_GET_VAR(NAME)		NAME
#define RESULT_LAST					RESULT_GET_VAR(result) ? *RESULT_GET_VAR(result) : eResultOk
#define RESULT_SET(VALUE)                                                                                              \
	if (result)                                                                                                        \
	RESULT_SET_VAR(*result, VALUE)
#define RESULT_VALUE_SET(VALUE)                                                                                              \
	RESULT_SET_VAR(result, VALUE)
#define RESULT_GET()				RESULT_GET_VAR(*result)
#define RESULT_LAST_COMPARE(RESULT) (RESULT_LAST == RESULT)
#define RESULT_OK_PTR				(Result)(eResultOk)
#define RESULT_OK()					RESULT_SET(eResultOk)
#define RESULT_ERROR(VALUE, ...)                                                                                       \
	RESULT_SET(VALUE);                                                                                                 \
	return __VA_ARGS__

#define RESULT_VALUE_ERROR(VALUE, ...)                                                                                       \
	RESULT_VALUE_SET(VALUE);                                                                                                 \
	return __VA_ARGS__

#define RESULT_PARAM_DEFINE	  RESULT* result = nullptr
#define RESULT_PARAM_IMPL	  RESULT* result
#define RESULT_ARG_PASS		  result
#define RESULT_ARG_VALUE_PASS &result

#define RESULT_CONDITION_ENSURE_NOLOG(EXP, RESULT, ...)                                                                \
	if (!(EXP))                                                                                                        \
	{                                                                                                                  \
		RESULT_ERROR(RESULT, __VA_ARGS__);                                                                             \
	}

#define RESULT_VALUE_CONDITION_ENSURE_NOLOG(EXP, RESULT, ...)                                                                \
	if (!(EXP))                                                                                                        \
	{                                                                                                                  \
		RESULT_VALUE_ERROR(RESULT, __VA_ARGS__);                                                                             \
	}

#define RESULT_CONDITION_ENSURE(EXP, RESULT, ...)                                                                      \
	if (!(EXP))                                                                                                        \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_YELLOW CONSOLE_COLOR_BOLD                                                           \
			   "RESULT ENSURE FAILED "                                                                                 \
			   " at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                           \
			   __FILE__, __LINE__, ResultString(RESULT), ResultInfo(RESULT));                                          \
		RESULT_ERROR(RESULT, __VA_ARGS__);                                                                             \
	}

#define RESULT_ENSURE_NOLOG(RESULT, ...)                                                                               \
	if ((result ? *result : RESULT_OK_PTR) != RESULT_OK_PTR)                                                           \
	{                                                                                                                  \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_VALUE_ENSURE_NOLOG(RESULT, ...)                                                                         \
	if (result != RESULT_OK_PTR)                                                                                       \
	{                                                                                                                  \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_ENSURE(RESULT, ...)                                                                                     \
	if ((result ? *result : RESULT_OK_PTR) != RESULT_OK_PTR)                                                           \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_YELLOW CONSOLE_COLOR_BOLD                                                           \
			   "ENSURE RESULT FAILED '" #RESULT                                                                        \
			   "' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                          \
			   __FILE__, __LINE__, ResultString(RESULT), ResultInfo(RESULT));                                          \
		RESULT_ERROR(RESULT, __VA_ARGS__);                                                                             \
	}

#define RESULT_VALUE_ENSURE(RESULT, ...)                                                                               \
	if (result != RESULT_OK_PTR)                                                                                       \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_YELLOW CONSOLE_COLOR_BOLD                                                           \
			   "ENSURE RESULT FAILED '" #RESULT                                                                        \
			   "' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                          \
			   __FILE__, __LINE__, ResultString(RESULT), ResultInfo(RESULT));                                          \
		RESULT_ERROR(RESULT, __VA_ARGS__);                                                                             \
	}

#define RESULT_ENSURE_LAST_NOLOG(...)                                                                                  \
	if ((result ? (*result != RESULT_OK_PTR) : false))                                                                 \
	{                                                                                                                  \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_VALUE_ENSURE_LAST_NOLOG(...)                                                                            \
	if (result != RESULT_OK_PTR)                                                                                       \
	{                                                                                                                  \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_ENSURE_LAST(...)                                                                                        \
	if ((result ? (*result != RESULT_OK_PTR) : false))                                                                 \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_YELLOW CONSOLE_COLOR_BOLD                                                           \
			   "ENSURE LAST RESULT FAILED "                                                                            \
			   " at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                           \
			   __FILE__, __LINE__, ResultString(*result), ResultInfo(*result));                                        \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_VALUE_ENSURE_LAST(...)                                                                                  \
	if (result != RESULT_OK_PTR)                                                                                       \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_YELLOW CONSOLE_COLOR_BOLD                                                           \
			   "ENSURE LAST RESULT FAILED "                                                                            \
			   " at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                           \
			   __FILE__, __LINE__, ResultString(result), ResultInfo(result));                                          \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_ENFORCE_NOLOG(RESULT)                                                                                   \
	if ((RESULT) != RESULT_OK_PTR)                                                                                     \
	{                                                                                                                  \
		abort();                                                                                                       \
	}

#define RESULT_ENFORCE(RESULT)                                                                                         \
	if ((RESULT) != RESULT_OK_PTR)                                                                                     \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD                                                              \
			   "ENFORCE RESULT FAILED '" #RESULT                                                                       \
			   "' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                          \
			   __FILE__, __LINE__, ResultString(RESULT), ResultInfo(RESULT));                                          \
		abort();                                                                                                       \
	}

#define RESULT_ENFORCE_LAST_NOLOG()                                                                                    \
	if ((result ? (*result != RESULT_OK_PTR) : false))                                                                 \
	{                                                                                                                  \
		abort();                                                                                                       \
	}

#define RESULT_VALUE_ENFORCE_LAST_NOLOG()                                                                              \
	if (result != RESULT_OK_PTR)                                                                                       \
	{                                                                                                                  \
		abort();                                                                                                       \
	}

#define RESULT_ENFORCE_LAST()                                                                                          \
	if ((result ? (*result != RESULT_OK_PTR) : false))                                                                 \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD                                                              \
			   "ENFORCE LAST RESULT FAILED "                                                                           \
			   " at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                           \
			   __FILE__, __LINE__, ResultString(*result), ResultInfo(*result));                                        \
		abort();                                                                                                       \
	}

#define RESULT_VALUE_ENFORCE_LAST()                                                                                    \
	if (result != RESULT_OK_PTR)                                                                                       \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD                                                              \
			   "ENFORCE LAST RESULT FAILED "                                                                           \
			   " at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                           \
			   __FILE__, __LINE__, ResultString(result), ResultInfo(result));                                          \
		abort();                                                                                                       \
	}

#define RESULT_ENSURE_CALL(SYMBOL, ...)                                                                                \
	SYMBOL;                                                                                                            \
	RESULT_ENSURE_LAST(__VA_ARGS__)

#define RESULT_ENSURE_CALL_NOLOG(SYMBOL, ...)                                                                          \
	SYMBOL;                                                                                                            \
	RESULT_ENSURE_LAST_NOLOG(__VA_ARGS__)

#define RESULT_ENFORCE_CALL(SYMBOL, ...)                                                                               \
	SYMBOL;                                                                                                            \
	RESULT_ENFORCE_LAST(__VA_ARGS__)

#define RESULT_ENFORCE_CALL_NOLOG(SYMBOL, ...)                                                                         \
	SYMBOL;                                                                                                            \
	RESULT_ENFORCE_LAST_NOLOG(__VA_ARGS__)

#define RESULT_VALUE_ENSURE_CALL(SYMBOL, ...)                                                                          \
	SYMBOL;                                                                                                            \
	RESULT_VALUE_ENSURE_LAST(__VA_ARGS__)

#define RESULT_VALUE_ENSURE_CALL_NOLOG(SYMBOL, ...)                                                                    \
	SYMBOL;                                                                                                            \
	RESULT_VALUE_ENSURE_LAST_NOLOG(__VA_ARGS__)

#define RESULT_VALUE_ENFORCE_CALL(SYMBOL, ...)                                                                         \
	SYMBOL;                                                                                                            \
	RESULT_VALUE_ENFORCE_LAST(__VA_ARGS__)

#define RESULT_VALUE_ENFORCE_CALL_NOLOG(SYMBOL, ...)                                                                   \
	SYMBOL;                                                                                                            \
	RESULT_VALUE_ENFORCE_LAST_NOLOG(__VA_ARGS__)

#ifndef RELEASE
#define RESULT_ASSERT_NOLOG(RESULT) RESULT_ENFORCE_NOLOG(RESULT)
#define RESULT_ASSERT(RESULT)		RESULT_ENFORCE(RESULT)
#else
#define RESULT_ASSERT_NOLOG(RESULT)
#define RESULT_ASSERT(RESULT)
#endif

/**
 * @brief Result enumeration.
 *
 * Contains all Result values.
 *
 */
enum Result : uint64_t
{
	/**
	 * @brief Used to Result pointers.
	 *
	 */
	eResultNull,
	/**
	 * @brief Ok.
	 */
	eResultOk,
	/**
	 * @brief Generic fail.
	 *
	 * Used when a function behaves like boolean.
	 *
	 */
	eResultFail,
	/**
	 * @brief Null pointer.
	 */
	eResultErrorNullPtr,
	/**
	 * @brief Pointer is not null.
	 *
	 * Used when a function need a pointer to be null.
	 *
	 */
	eResultErrorPtrIsNotNull,
	/**
	 * @brief Zero size.
	 *
	 * Used by functions that need manipulate buffer and the size always need to be greater than zero.
	 *
	 */
	eResultErrorZeroSize,
	eResultErrorZeroTime,

	eResultErrorNotImplemented,

	/**
	 * @brief Element not found.
	 *
	 * Generic error used to find element in container.
	 *
	 */
	eResultErrorElementNotFound,

	/**
	 * @brief Out of memory.
	 *
	 * Used when the OS function to allocate fail by lack of memory.
	 *
	 */
	eResultErrorMemoryOutOfMemory,
	/**
	 * @brief Comparation between size 1 and 2 get fail.
	 *
	 * Used by reallocate function when old size is equal new size.
	 * It doesn't make sense reallocate memory when the sizes are equal.
	 *
	 */
	eResultErrorMemoryInvalidSizes,

	/**
	 * @brief Not enough buffer memory.
	 *
	 * Used in memory manipulation functions when a destination buffer has less memory
	 * than source buffer.
	 *
	 */
	eResultErrorMemoryNotEnoughBufferMemory,

	eResultErrorMemoryOutOfBuffer,

	eResultErrorStreamInvalidSizes,
	eResultErrorStreamFailedToWrite,
	eResultErrorStreamFailedToRead,

	/**
	 * @brief Exceeded max path length.
	 *
	 * Used when a Path string is greater than the allowed by the platform.
	 *
	 */
	eResultErrorIoExceededMaxPathLength,

	/**
	 * @brief File open invalid flags.
	 *
	 */
	eResultErrorIoFileInvalidFlags,
	/**
	 * @brief File open failed.
	 *
	 */
	eResultErrorIoFileOpenFailed,
	/**
	 * @brief File stream read failed.
	 *
	 */
	eResultErrorIoFileReadFailed,
	/**
	 * @brief File write failed.
	 *
	 */
	eResultErrorIoFileWriteFailed,
	/**
	 * @brief File seek failed.
	 *
	 */
	eResultErrorIoFileSeekFailed,
	/**
	 * @brief File flush failed.
	 *
	 */
	eResultErrorIoFileFlushFailed,
	/**
	 * @brief File close failed.
	 *
	 */
	eResultErrorIoFileCloseFailed,

	eResultErrorThreadCreateFailed,
	eResultErrorThreadDestroyFailed,
	eResultErrorThreadNativeParamsDestroyFailed,
	eResultErrorThreadSleepFailed,
	eResultErrorThreadSuspendFailed,
	eResultErrorThreadResumeFailed,
	eResultErrorThreadAffinityFailed,
	eResultErrorThreadWaitFailed,

	eResultErrorMutexCreateFailed,
	eResultErrorMutexDestroyFailed,
	eResultErrorMutexLockFailed,
	eResultErrorMutexUnlockFailed,

	/**
	 * @brief Feature not allowed.
	 *
	 * Used when an allocator cannot call a function because of its limitation.
	 *
	 */
	eResultErrorAllocatorFeatureNotAllowed,

	/**
	 * @brief Out of memory buffer.
	 *
	 * Used when an allocator try to read/write at fixed buffer and the buffer does not have available size.
	 *
	 */
	eResultErrorAllocatorOutOfMemoryBuffer,

	eResultErrorEcsComponentAlreadyEnabled,
	eResultErrorEcsComponentNotEnabled,
	eResultErrorEcsComponentDataAddedMoreThanOnce,
	eResultErrorEcsComponentDataNotAdded,
	eResultErrorEcsInvalidEntityId,
	eResultErrorEcsInvalidComponentIndex,
	eResultErrorEcsNoEntityAvailable,

	eResultErrorAssetFailedToAdd,
	eResultErrorAssetLoadFailedInvalidFile,
	eResultErrorAssetObjectLoadFailed,
	eResultErrorAssetAlreadyLoaded,
	eResultErrorAssetWasNotLoaded,
	eResultErrorAssetFailedToImport,
	eResultErrorAssetFailedToOpenRegistryFile,

	eResultErrorAlreadyInitialized,
	eResultErrorNotInitialized,

	eResultErrorPlatformRenderUnsupportedExtension,
	eResultErrorPlatformRenderUnsupportedLayer,
	eResultErrorPlatformRenderFailedToCreateInstance,
	eResultErrorPlatformRenderFailedToCreateDebugUtils,
	eResultErrorPlatformRenderUnsuitableGpu,
	eResultErrorPlatformRenderFailedToCreateDevice,
	eResultErrorPlatformRenderFailedToCreateSurface,
	eResultErrorPlatformRenderFailedToCreateSwapChain
};

/**
 * @brief Result string function.
 *
 * Get the string of a given Result value.
 *
 * @param value : result value.
 * @return result string.
 *
 */
static constexpr char* ResultString(const Result value)
{
#define RESULT_STRING_CASE_IMPL(NAME)                                                                                  \
	case eResult##NAME:                                                                                                \
		return (char*)#NAME

	switch (value)
	{
		RESULT_STRING_CASE_IMPL(Ok);
		RESULT_STRING_CASE_IMPL(Fail);
		RESULT_STRING_CASE_IMPL(ErrorNullPtr);
		RESULT_STRING_CASE_IMPL(ErrorZeroSize);
		RESULT_STRING_CASE_IMPL(ErrorZeroTime);
		RESULT_STRING_CASE_IMPL(ErrorPtrIsNotNull);

		RESULT_STRING_CASE_IMPL(ErrorMemoryOutOfMemory);
		RESULT_STRING_CASE_IMPL(ErrorMemoryNotEnoughBufferMemory);
		RESULT_STRING_CASE_IMPL(ErrorMemoryOutOfBuffer);

		RESULT_STRING_CASE_IMPL(ErrorStreamInvalidSizes);
		RESULT_STRING_CASE_IMPL(ErrorStreamFailedToWrite);
		RESULT_STRING_CASE_IMPL(ErrorStreamFailedToRead);

		RESULT_STRING_CASE_IMPL(ErrorIoExceededMaxPathLength);

		RESULT_STRING_CASE_IMPL(ErrorIoFileInvalidFlags);
		RESULT_STRING_CASE_IMPL(ErrorIoFileOpenFailed);
		RESULT_STRING_CASE_IMPL(ErrorIoFileReadFailed);
		RESULT_STRING_CASE_IMPL(ErrorIoFileWriteFailed);
		RESULT_STRING_CASE_IMPL(ErrorIoFileSeekFailed);
		RESULT_STRING_CASE_IMPL(ErrorIoFileFlushFailed);
		RESULT_STRING_CASE_IMPL(ErrorIoFileCloseFailed);

		RESULT_STRING_CASE_IMPL(ErrorEcsComponentAlreadyEnabled);
		RESULT_STRING_CASE_IMPL(ErrorEcsComponentNotEnabled);
		RESULT_STRING_CASE_IMPL(ErrorEcsInvalidEntityId);
		RESULT_STRING_CASE_IMPL(ErrorEcsNoEntityAvailable);

		RESULT_STRING_CASE_IMPL(ErrorAssetFailedToAdd);
		RESULT_STRING_CASE_IMPL(ErrorAssetLoadFailedInvalidFile);
		RESULT_STRING_CASE_IMPL(ErrorAssetAlreadyLoaded);
		RESULT_STRING_CASE_IMPL(ErrorAssetWasNotLoaded);
		RESULT_STRING_CASE_IMPL(ErrorAssetFailedToImport);

		RESULT_STRING_CASE_IMPL(ErrorAlreadyInitialized);
		RESULT_STRING_CASE_IMPL(ErrorNotInitialized);

		RESULT_STRING_CASE_IMPL(ErrorPlatformRenderUnsupportedExtension);
		RESULT_STRING_CASE_IMPL(ErrorPlatformRenderUnsupportedLayer);
		RESULT_STRING_CASE_IMPL(ErrorPlatformRenderFailedToCreateInstance);
		RESULT_STRING_CASE_IMPL(ErrorPlatformRenderFailedToCreateDebugUtils);
		RESULT_STRING_CASE_IMPL(ErrorPlatformRenderUnsuitableGpu);
		RESULT_STRING_CASE_IMPL(ErrorPlatformRenderFailedToCreateDevice);
		RESULT_STRING_CASE_IMPL(ErrorPlatformRenderFailedToCreateSurface);
		RESULT_STRING_CASE_IMPL(ErrorPlatformRenderFailedToCreateSwapChain);
	default:;
	}
	return (char*)"INVALID RESULT";
}

/**
 * @brief Result info function.
 *
 * Get information about a given Result value.
 *
 * @param value
 * @return
 */
static constexpr char* ResultInfo(const Result value)
{
#define RESULT_INFO_CASE_IMPL(NAME, MSG)                                                                               \
	case eResult##NAME:                                                                                                \
		return (char*)MSG

	switch (value)
	{
		RESULT_INFO_CASE_IMPL(Ok, "Ok.");
		RESULT_INFO_CASE_IMPL(Fail, "Generic fail. \n"
									"Used when a function behaves like boolean.");
		RESULT_INFO_CASE_IMPL(ErrorNullPtr, "Null pointer.");
		RESULT_INFO_CASE_IMPL(ErrorPtrIsNotNull,
							  "Pointer is not null. Used when a function need a pointer to be null.");
		RESULT_INFO_CASE_IMPL(ErrorZeroSize, "Zero size. Used by functions that need manipulate buffer and the size \n"
											 "always need to be greater than zero.");
		RESULT_INFO_CASE_IMPL(ErrorMemoryOutOfMemory,
							  "Out of memory. Used when the OS function to allocate fail by lack of memory.");
		RESULT_INFO_CASE_IMPL(ErrorMemoryInvalidSizes,
							  "Comparation between size 1 and 2 get fail. \n"
							  "Used by reallocate function when old size is equal new size. \n"
							  "It doesn't make sense reallocate memory when the sizes are equal.");
		RESULT_INFO_CASE_IMPL(ErrorMemoryNotEnoughBufferMemory,
							  "Not enough buffer memory."
							  "Used in memory manipulation functions when a destination buffer has less memory "
							  "than source buffer.");
		RESULT_INFO_CASE_IMPL(ErrorIoExceededMaxPathLength,
							  "Exceeded max Path length. \n"
							  "Used when a Path string is greater than the allowed by the platform.");
		RESULT_INFO_CASE_IMPL(ErrorIoFileInvalidFlags, "File open invalid flags");
		RESULT_INFO_CASE_IMPL(ErrorIoFileOpenFailed, "File open failed");
		RESULT_INFO_CASE_IMPL(ErrorIoFileReadFailed, "File read failed");
		RESULT_INFO_CASE_IMPL(ErrorIoFileWriteFailed, "File write failed");
		RESULT_INFO_CASE_IMPL(ErrorIoFileSeekFailed, "File seek failed");
		RESULT_INFO_CASE_IMPL(ErrorIoFileFlushFailed, "File flush failed");
		RESULT_INFO_CASE_IMPL(ErrorIoFileCloseFailed, "File close failed");
	default:;
	}
	return (char*)"INVALID RESULT";
}

#endif
