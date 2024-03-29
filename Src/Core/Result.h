
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
#include "Editor/Common.h"
#include "Core/Stacktrace.h"

#if _MSC_VER
#pragma warning(disable : 4100)
#endif

#define RESULT EResult
#define RESULT_VAR(NAME)                                                                                               \
	RESULT* NAME = nullptr;                                                                                            \
	(void)NAME

#define RESULT_VALUE_VAR(NAME)                                                                                         \
	RESULT NAME = Ok;                                                                                                  \
	(void)NAME
#define RESULT_UNUSED()				(void)result
#define RESULT_SET_VAR(NAME, VALUE) NAME = (EResult)(VALUE)
#define RESULT_GET_VAR(NAME)		NAME
#define RESULT_LAST					RESULT_GET_VAR(result) ? *RESULT_GET_VAR(result) : Ok
#define RESULT_SET(VALUE)                                                                                              \
	if (result)                                                                                                        \
	RESULT_SET_VAR(*result, VALUE)
#define RESULT_VALUE_SET(VALUE)		RESULT_SET_VAR(result, VALUE)
#define RESULT_GET()				RESULT_GET_VAR(*result)
#define RESULT_LAST_COMPARE(RESULT) (RESULT_LAST == RESULT)
#define RESULT_OK_VALUE				(EResult)(Ok)
#define RESULT_OK()					RESULT_SET(Ok)
#define RESULT_ERROR(VALUE, ...)                                                                                       \
	RESULT_SET(VALUE);                                                                                                 \
	return __VA_ARGS__

#define RESULT_VALUE_ERROR(VALUE, ...)                                                                                 \
	RESULT_VALUE_SET(VALUE);                                                                                           \
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

#define RESULT_VALUE_CONDITION_ENSURE_NOLOG(EXP, RESULT, ...)                                                          \
	if (!(EXP))                                                                                                        \
	{                                                                                                                  \
		RESULT_VALUE_ERROR(RESULT, __VA_ARGS__);                                                                       \
	}

#define RESULT_CONDITION_ENSURE(EXP, RESULT, ...)                                                                      \
	if (!(EXP))                                                                                                        \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD                                                              \
			   "%s: RESULT ENSURE FAILED "                                                                             \
			   " at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                           \
			   __FUNCTION__, __FILE__, __LINE__, ResultString(RESULT), ResultInfo(RESULT));                            \
		printf("Stack trace >>\n%s", GetStackTraceString().c_str());                                                   \
		RESULT_ERROR(RESULT, __VA_ARGS__);                                                                             \
	}

#define RESULT_ENSURE_NOLOG(RESULT, ...)                                                                               \
	if ((result ? *result : RESULT_OK_VALUE) != RESULT_OK_VALUE)                                                       \
	{                                                                                                                  \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_VALUE_ENSURE_NOLOG(RESULT, ...)                                                                         \
	if (result != RESULT_OK_VALUE)                                                                                     \
	{                                                                                                                  \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_ENSURE(RESULT, ...)                                                                                     \
	if ((result ? *result : RESULT_OK_VALUE) != RESULT_OK_VALUE)                                                       \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD                                                              \
			   "%s: ENSURE RESULT FAILED '" #RESULT                                                                    \
			   "' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                          \
			   __FUNCTION__, __FILE__, __LINE__, ResultString(RESULT), ResultInfo(RESULT));                            \
		RESULT_ERROR(RESULT, __VA_ARGS__);                                                                             \
	}

#define RESULT_VALUE_ENSURE(RESULT, ...)                                                                               \
	if (result != RESULT_OK_VALUE)                                                                                     \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD                                                              \
			   "%s: ENSURE RESULT FAILED '" #RESULT                                                                    \
			   "' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                          \
			   __FUNCTION__, __FILE__, __LINE__, ResultString(RESULT), ResultInfo(RESULT));                            \
		RESULT_ERROR(RESULT, __VA_ARGS__);                                                                             \
	}

#define RESULT_ENSURE_LAST_NOLOG(...)                                                                                  \
	if ((result ? (*result != RESULT_OK_VALUE) : false))                                                               \
	{                                                                                                                  \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_VALUE_ENSURE_LAST_NOLOG(...)                                                                            \
	if (result != RESULT_OK_VALUE)                                                                                     \
	{                                                                                                                  \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_ENSURE_LAST(...)                                                                                        \
	if ((result ? (*result != RESULT_OK_VALUE) : false))                                                               \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD                                                              \
			   "%s: ENSURE LAST RESULT FAILED "                                                                        \
			   " at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                           \
			   __FUNCTION__, __FILE__, __LINE__, ResultString(*result), ResultInfo(*result));                          \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_ENSURE_LAST_NOLOG(...)                                                                                  \
	if ((result ? (*result != RESULT_OK_VALUE) : false))                                                               \
	{                                                                                                                  \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_VALUE_ENSURE_LAST(...)                                                                                  \
	if (result != RESULT_OK_VALUE)                                                                                     \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD                                                              \
			   "%s: ENSURE LAST RESULT FAILED "                                                                        \
			   " at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                           \
			   __FUNCTION__, __FILE__, __LINE__, ResultString(result), ResultInfo(result));                            \
		return __VA_ARGS__;                                                                                            \
	}

#define RESULT_ENFORCE_NOLOG(RESULT)                                                                                   \
	if ((RESULT) != RESULT_OK_VALUE)                                                                                   \
	{                                                                                                                  \
		abort();                                                                                                       \
	}

#define RESULT_ENFORCE(RESULT)                                                                                         \
	if ((RESULT) != RESULT_OK_VALUE)                                                                                   \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_RED CONSOLE_COLOR_BOLD                                                                    \
			   "%s: ENFORCE RESULT FAILED '" #RESULT                                                                   \
			   "' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                          \
			   __FUNCTION__, __FILE__, __LINE__, ResultString(RESULT), ResultInfo(RESULT));                            \
		abort();                                                                                                       \
	}

#define RESULT_ENFORCE_LAST_NOLOG()                                                                                    \
	if ((result ? (*result != RESULT_OK_VALUE) : false))                                                               \
	{                                                                                                                  \
		abort();                                                                                                       \
	}

#define RESULT_VALUE_ENFORCE_LAST_NOLOG()                                                                              \
	if (result != RESULT_OK_VALUE)                                                                                     \
	{                                                                                                                  \
		abort();                                                                                                       \
	}

#define RESULT_ENFORCE_LAST()                                                                                          \
	if ((result ? (*result != RESULT_OK_VALUE) : false))                                                               \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_RED CONSOLE_COLOR_BOLD                                                                    \
			   "%s: ENFORCE LAST RESULT FAILED "                                                                       \
			   " at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                           \
			   __FUNCTION__, __FILE__, __LINE__, ResultString(*result), ResultInfo(*result));                          \
		abort();                                                                                                       \
	}

#define RESULT_VALUE_ENFORCE_LAST()                                                                                    \
	if (result != RESULT_OK_VALUE)                                                                                     \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_RED CONSOLE_COLOR_BOLD                                                                    \
			   "%s: ENFORCE LAST RESULT FAILED "                                                                       \
			   " at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT,                           \
			   __FUNCTION__, __FILE__, __LINE__, ResultString(result), ResultInfo(result));                            \
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
enum EResult : uint64_t
{
	/**
	 * @brief Used to Result pointers.
	 *
	 */
	Null,
	/**
	 * @brief Ok.
	 */
	Ok,
	/**
	 * @brief Generic fail.
	 *
	 * Used when a function behaves like boolean.
	 *
	 */
	Fail,
	/**
	 * @brief Null pointer.
	 */
	NullPtr,
	/**
	 * @brief Pointer is not null.
	 *
	 * Used when a function need a pointer to be null.
	 *
	 */
	PtrIsNotNull,
	/**
	 * @brief Zero size.
	 *
	 * Used by functions that need manipulate buffer and the size always need to be greater than zero.
	 *
	 */
	ZeroSize,
	ZeroTime,

	NotImplemented,
	InvalidIndex,

	/**
	 * @brief Element not found.
	 *
	 * Generic error used to find element in container.
	 *
	 */
	ElementNotFound,

	/**
	 * @brief Out of memory.
	 *
	 * Used when the OS function to allocate fail by lack of memory.
	 *
	 */
	MemoryOutOfMemory,
	/**
	 * @brief Comparation between size 1 and 2 get fail.
	 *
	 * Used by reallocate function when old size is equal new size.
	 * It doesn't make sense reallocate memory when the sizes are equal.
	 *
	 */
	MemoryInvalidSizes,

	/**
	 * @brief Not enough buffer memory.
	 *
	 * Used in memory manipulation functions when a destination buffer has less memory
	 * than source buffer.
	 *
	 */
	MemoryNotEnoughBufferMemory,

	MemoryOutOfBuffer,

	StreamInvalidSizes,
	StreamFailedToWrite,
	StreamFailedToRead,

	/**
	 * @brief Exceeded max path length.
	 *
	 * Used when a Path string is greater than the allowed by the platform.
	 *
	 */
	IoExceededMaxPathLength,

	/**
	 * @brief File open invalid flags.
	 *
	 */
	IoFileInvalidFlags,
	/**
	 * @brief File open failed.
	 *
	 */
	IoFileOpenFailed,
	/**
	 * @brief File stream read failed.
	 *
	 */
	IoFileReadFailed,
	/**
	 * @brief File write failed.
	 *
	 */
	IoFileWriteFailed,
	/**
	 * @brief File seek failed.
	 *
	 */
	IoFileSeekFailed,
	/**
	 * @brief File flush failed.
	 *
	 */
	IoFileFlushFailed,
	/**
	 * @brief File close failed.
	 *
	 */
	IoFileCloseFailed,

	ThreadCreateFailed,
	ThreadDestroyFailed,
	ThreadNativeParamsDestroyFailed,
	ThreadSleepFailed,
	ThreadSuspendFailed,
	ThreadResumeFailed,
	ThreadAffinityFailed,
	ThreadWaitFailed,
	CurrentThreadIsNotTheRequiredOne,

	MutexCreateFailed,
	MutexDestroyFailed,
	MutexLockFailed,
	MutexUnlockFailed,

	/**
	 * @brief Feature not allowed.
	 *
	 * Used when an allocator cannot call a function because of its limitation.
	 *
	 */
	AllocatorFeatureNotAllowed,

	/**
	 * @brief Out of memory buffer.
	 *
	 * Used when an allocator try to read/write at fixed buffer and the buffer does not have available size.
	 *
	 */
	AllocatorOutOfMemoryBuffer,

	EcsComponentAlreadyEnabled,
	EcsComponentNotEnabled,
	EcsComponentDataAddedMoreThanOnce,
	EcsComponentDataNotAdded,
	EcsInvalidEntityId,
	EcsInvalidComponentIndex,
	EcsNoEntityAvailable,

	AssetFailedToAdd,
	AssetLoadFailedInvalidFile,
	AssetObjectLoadFailed,
	AssetAlreadyLoaded,
	AssetWasNotLoaded,
	AssetFailedToImport,
	AssetFailedToOpenRegistryFile,

	AlreadyInitialized,
	NotInitialized,

	RenderPlatformFailedToInitialize,
	RenderPlatformFailedToUpdate,
	RenderPlatformFailedToCompileShader,

#if PLATFORM_WINDOWS
#if OPENGL_ENABLED
	OpenglFailedToInitializeGlew,
	OpenglFailedToRequiredExtensionIsNotSupported,
#elif VULKAN_ENABLED
	VulkanFailedToCreateInstance,
	VulkanFailedToSetupDebugMessenger,
	VulkanFailedToFindGpuWithVulkanSupport,
	VulkanFailedToPickPhysicalDevice,
	VulkanFailedToCreateDevice,
	VulkanFailedToCreateSurface,
	VulkanFailedToCreateSwapchain,
	VulkanFailedToCreateSwapchainImageView,
	VulkanFailedToCreateShaderModule,
	VulkanFailedToCreateGraphicsPipeline,
	VulkanFailedToCreateGraphicsPipelineLayout,
	VulkanFailedToCreateRenderPass,
	VulkanFailedToCreateFramebuffer,
	VulkanFailedToCreateCommandPool,
	VulkanFailedToCreateCommandBuffer,
	VulkanFailedToBeginCommandBuffer,
	VulkanFailedToEndCommandBuffer,
	VulkanFailedToCreateSemaphore,
	VulkanFailedToCreateFence,
	VulkanFailedToSubmitGraphicsQueue,
	VulkanFailedToSubmitTransferQueue,
	VulkanFailedToAcquireSwapchainImage,
	VulkanFailedToCreateBuffer,
	VulkanFailedToAllocateMemory,
	VulkanUnexpectedBufferUsage,
	VulkanFailedToWaitQueueIdle,
	VulkanFailedToCreateDescriptoSetLayout,
	VulkanFailedToCreateDescriptorPool,
	VulkanFailedToAllocateDescriptorSets,
#endif
#endif

#if EDITOR
	EditorFailedToInitImguiWindow,
	EditorFailedToInitImguiBackend,
#endif
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
static constexpr char* ResultString(const EResult Value)
{
#ifdef DEBUG
#define RESULT_STRING_CASE_IMPL(NAME)                                                                                  \
	case NAME:                                                                                                         \
		return (char*)#NAME

	switch (Value)
	{
		RESULT_STRING_CASE_IMPL(Ok);
		RESULT_STRING_CASE_IMPL(Fail);
		RESULT_STRING_CASE_IMPL(NullPtr);
		RESULT_STRING_CASE_IMPL(ZeroSize);
		RESULT_STRING_CASE_IMPL(ZeroTime);
		RESULT_STRING_CASE_IMPL(PtrIsNotNull);
		RESULT_STRING_CASE_IMPL(InvalidIndex);

		RESULT_STRING_CASE_IMPL(MemoryOutOfMemory);
		RESULT_STRING_CASE_IMPL(MemoryNotEnoughBufferMemory);
		RESULT_STRING_CASE_IMPL(MemoryOutOfBuffer);

		RESULT_STRING_CASE_IMPL(StreamInvalidSizes);
		RESULT_STRING_CASE_IMPL(StreamFailedToWrite);
		RESULT_STRING_CASE_IMPL(StreamFailedToRead);

		RESULT_STRING_CASE_IMPL(IoExceededMaxPathLength);

		RESULT_STRING_CASE_IMPL(IoFileInvalidFlags);
		RESULT_STRING_CASE_IMPL(IoFileOpenFailed);
		RESULT_STRING_CASE_IMPL(IoFileReadFailed);
		RESULT_STRING_CASE_IMPL(IoFileWriteFailed);
		RESULT_STRING_CASE_IMPL(IoFileSeekFailed);
		RESULT_STRING_CASE_IMPL(IoFileFlushFailed);
		RESULT_STRING_CASE_IMPL(IoFileCloseFailed);

		RESULT_STRING_CASE_IMPL(ThreadCreateFailed);
		RESULT_STRING_CASE_IMPL(ThreadDestroyFailed);
		RESULT_STRING_CASE_IMPL(ThreadNativeParamsDestroyFailed);
		RESULT_STRING_CASE_IMPL(ThreadSleepFailed);
		RESULT_STRING_CASE_IMPL(ThreadSuspendFailed);
		RESULT_STRING_CASE_IMPL(ThreadResumeFailed);
		RESULT_STRING_CASE_IMPL(ThreadAffinityFailed);
		RESULT_STRING_CASE_IMPL(ThreadWaitFailed);
		RESULT_STRING_CASE_IMPL(CurrentThreadIsNotTheRequiredOne);

		RESULT_STRING_CASE_IMPL(MutexCreateFailed);
		RESULT_STRING_CASE_IMPL(MutexDestroyFailed);
		RESULT_STRING_CASE_IMPL(MutexLockFailed);
		RESULT_STRING_CASE_IMPL(MutexUnlockFailed);

		RESULT_STRING_CASE_IMPL(EcsComponentAlreadyEnabled);
		RESULT_STRING_CASE_IMPL(EcsComponentNotEnabled);
		RESULT_STRING_CASE_IMPL(EcsInvalidEntityId);
		RESULT_STRING_CASE_IMPL(EcsNoEntityAvailable);

		RESULT_STRING_CASE_IMPL(AssetFailedToAdd);
		RESULT_STRING_CASE_IMPL(AssetLoadFailedInvalidFile);
		RESULT_STRING_CASE_IMPL(AssetAlreadyLoaded);
		RESULT_STRING_CASE_IMPL(AssetWasNotLoaded);
		RESULT_STRING_CASE_IMPL(AssetFailedToImport);

		RESULT_STRING_CASE_IMPL(AlreadyInitialized);
		RESULT_STRING_CASE_IMPL(NotInitialized);

		RESULT_STRING_CASE_IMPL(RenderPlatformFailedToInitialize);
		RESULT_STRING_CASE_IMPL(RenderPlatformFailedToUpdate);
		RESULT_STRING_CASE_IMPL(RenderPlatformFailedToCompileShader);

#if PLATFORM_WINDOWS
#if OPENGL_ENABLED
		RESULT_STRING_CASE_IMPL(OpenglFailedToInitializeGlew);
		RESULT_STRING_CASE_IMPL(OpenglFailedToRequiredExtensionIsNotSupported);
#elif VULKAN_ENABLED
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateInstance);
		RESULT_STRING_CASE_IMPL(VulkanFailedToSetupDebugMessenger);
		RESULT_STRING_CASE_IMPL(VulkanFailedToFindGpuWithVulkanSupport);
		RESULT_STRING_CASE_IMPL(VulkanFailedToPickPhysicalDevice);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateDevice);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateSurface);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateSwapchain);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateSwapchainImageView);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateShaderModule);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateGraphicsPipeline);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateGraphicsPipelineLayout);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateRenderPass);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateFramebuffer);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateCommandPool);
		RESULT_STRING_CASE_IMPL(VulkanFailedToBeginCommandBuffer);
		RESULT_STRING_CASE_IMPL(VulkanFailedToAcquireSwapchainImage);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateBuffer);
		RESULT_STRING_CASE_IMPL(VulkanFailedToAllocateMemory);
		RESULT_STRING_CASE_IMPL(VulkanUnexpectedBufferUsage);
		RESULT_STRING_CASE_IMPL(VulkanFailedToSubmitGraphicsQueue);
		RESULT_STRING_CASE_IMPL(VulkanFailedToSubmitTransferQueue);
		RESULT_STRING_CASE_IMPL(VulkanFailedToWaitQueueIdle);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateDescriptoSetLayout);
		RESULT_STRING_CASE_IMPL(VulkanFailedToCreateDescriptorPool);
		RESULT_STRING_CASE_IMPL(VulkanFailedToAllocateDescriptorSets);
#endif
#endif

#if EDITOR
		RESULT_STRING_CASE_IMPL(EditorFailedToInitImguiWindow);
		RESULT_STRING_CASE_IMPL(EditorFailedToInitImguiBackend);
#endif
	default:;
	}
	return (char*)"INVALID RESULT";
#else
	return (char*)"NONE";
#endif
}

/**
 * @brief Result info function.
 *
 * Get information about a given Result value.
 *
 * @param Value
 * @return
 */
static constexpr char* ResultInfo(const EResult Value)
{
#ifdef DEBUG
#define RESULT_INFO_CASE_IMPL(NAME, MSG)                                                                               \
	case NAME:                                                                                                         \
		return (char*)MSG

	switch (Value)
	{
		RESULT_INFO_CASE_IMPL(Ok, "Ok.");
		RESULT_INFO_CASE_IMPL(Fail, "Generic fail. \n"
									"Used when a function behaves like boolean.");
		RESULT_INFO_CASE_IMPL(NullPtr, "Null pointer.");
		RESULT_INFO_CASE_IMPL(PtrIsNotNull, "Pointer is not null. Used when a function need a pointer to be null.");
		RESULT_INFO_CASE_IMPL(ZeroSize, "Zero size. Used by functions that need manipulate buffer and the size \n"
										"always need to be greater than zero.");
		RESULT_INFO_CASE_IMPL(MemoryOutOfMemory,
							  "Out of memory. Used when the OS function to allocate fail by lack of memory.");
		RESULT_INFO_CASE_IMPL(MemoryInvalidSizes, "Comparation between size 1 and 2 get fail. \n"
												  "Used by reallocate function when old size is equal new size. \n"
												  "It doesn't make sense reallocate memory when the sizes are equal.");
		RESULT_INFO_CASE_IMPL(MemoryNotEnoughBufferMemory,
							  "Not enough buffer memory."
							  "Used in memory manipulation functions when a destination buffer has less memory "
							  "than source buffer.");
		RESULT_INFO_CASE_IMPL(IoExceededMaxPathLength,
							  "Exceeded max Path length. \n"
							  "Used when a Path string is greater than the allowed by the platform.");
		RESULT_INFO_CASE_IMPL(IoFileInvalidFlags, "File open invalid flags");
		RESULT_INFO_CASE_IMPL(IoFileOpenFailed, "File open failed");
		RESULT_INFO_CASE_IMPL(IoFileReadFailed, "File read failed");
		RESULT_INFO_CASE_IMPL(IoFileWriteFailed, "File write failed");
		RESULT_INFO_CASE_IMPL(IoFileSeekFailed, "File seek failed");
		RESULT_INFO_CASE_IMPL(IoFileFlushFailed, "File flush failed");
		RESULT_INFO_CASE_IMPL(IoFileCloseFailed, "File close failed");
	default:;
	}
	return (char*)"INVALID RESULT";
#else
	return (char*)"NONE";
#endif
}

#endif
