
#ifndef RESULT_H
#define RESULT_H

#include "Core/Platform.h"

#define RESULT_IS_OK				eResultOk == g_result
#define RESULT_IS_NOT_OK			eResultOk != g_result
#define RESULT_LAST					LastResult()
#define RESULT_SET(VALUE)			RESULT_LAST = VALUE
#define RESULT_OK()					RESULT_SET(eResultOk)
#define RESULT_ERROR(VALUE, ...)	RESULT_SET(VALUE); return __VA_ARGS__

#define ENSURE_RESULT_NL(RESULT, ...)	\
if((RESULT_LAST=RESULT) != eResultOk) { return __VA_ARGS__; }

#define ENSURE_RESULT(RESULT, ...)	\
if((RESULT_LAST=RESULT) != eResultOk) { printf(CONSOLE_COLOR_LIGHT_YELLOW CONSOLE_COLOR_BOLD	\
	"ENSURE RESULT FAILED '" #RESULT \
	"' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT, __FILE__, __LINE__, \
	resultString(RESULT), resultInfo(RESULT)); return __VA_ARGS__; }

#define ENSURE_LAST_RESULT_NL(...)	\
if(RESULT_LAST != eResultOk) { return __VA_ARGS__; }

#define ENSURE_LAST_RESULT(...)	\
if(RESULT_LAST != eResultOk) { printf(CONSOLE_COLOR_LIGHT_YELLOW CONSOLE_COLOR_BOLD	\
	"ENSURE RESULT FAILED '" "RESULT_LAST" \
	"' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT, __FILE__, __LINE__, \
	resultString(RESULT_LAST), resultInfo(RESULT_LAST)); return __VA_ARGS__; }

#define ENFORCE_RESULT_NL(RESULT)	\
if((LastResult()=RESULT) != eResultOk) { abort(); }

#define ENFORCE_RESULT(RESULT)	\
if((LastResult()=RESULT) != eResultOk) { printf(LOG_COLOR_LIGHT_RED LOG_COLOR_BOLD	\
	"ENFORCE RESULT FAILED '" #RESULT \
	"' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" LOG_COLOR_DEFAULT, __FILE__, __LINE__, \
	resultString(RESULT), resultInfo(RESULT)); abort(); }

#define ENFORCE_LAST_RESULT_NL()	\
if(RESULT_LAST != eResultOk) { abort(); }

#define ENFORCE_LAST_RESULT()	\
if(RESULT_LAST != eResultOk) { printf(CONSOLE_COLOR_LIGHT_YELLOW CONSOLE_COLOR_BOLD	\
	"ENFORCE RESULT FAILED '" "RESULT_LAST" \
	"' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT, __FILE__, __LINE__, \
	resultString(RESULT_LAST), resultInfo(RESULT_LAST)); abort(); }

#define RESULT_ENSURE_CALL(FUNCTION, ...)	\
FUNCTION;	\
ENSURE_LAST_RESULT(__VA_ARGS__)

#define RESULT_ENSURE_CALL_NL(FUNCTION, ...)	\
FUNCTION;	\
ENSURE_LAST_RESULT_NL(__VA_ARGS__)

#define RESULT_ENFORCE_CALL(FUNCTION, ...)	\
FUNCTION;	\
ENFORCE_LAST_RESULT(__VA_ARGS__)

#define RESULT_ENFORCE_CALL_NL(FUNCTION, ...)	\
FUNCTION;	\
ENFORCE_LAST_RESULT_NL(__VA_ARGS__)

#ifndef NDEBUG
#define ASSERT_NL(EXP)					ENFORCE_NL(EXP)
#define ASSERT(EXP)						ENFORCE(EXP)
#define ASSERT_RESULT_NL(RESULT)		ENFORCE_RESULT_NL(RESULT)
#define ASSERT_RESULT(RESULT)			ENFORCE_RESULT(RESULT)
#else
#define ASSERT_NL(EXP)
#define ASSERT(EXP)
#define ASSERT_RESULT_NL(RESULT)
#define ASSERT_RESULT(RESULT)
#endif

/**
 * @brief Result enumeration.
 * 
 * Contains all Result values.
 * 
*/
enum Result
{
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
	eResultErrorEcsNoEntityAvailable
};

inline Result& LastResult()
{
	static Result l_value;
	return l_value;
}

template < typename Owner, typename ... Args >
void resulTransfer(Result& result, 
	Owner* owner,
	Result (Owner::*function)(Args&&...), 
	Args&&... args)
{
	result = owner->*function(static_cast<Args&&>(args)...);
}

template < typename Owner, typename R, typename ... Args >
void resultTransferSetOnlyFailures(Result& result, 
	Owner* owner, 
	R (Owner::*function)(Args&&...), 
	Args&&... args)
{
	if(const Result l_result = owner->*function(static_cast<Args&&>(args)...); 
		l_result != eResultOk)
	{
		result = l_result;
	}
}

/**
 * @brief Result string function.
 * 
 * Get the string of a given Result value.
 * 
 * @param value : result value.
 * @return result string.
 * 
*/
static constexpr char* resultString(const Result value)
{
#define RESULT_STRING_CASE_IMPL(NAME)	\
	case eResult##NAME: return (char*)#NAME

	switch(value)
	{
	RESULT_STRING_CASE_IMPL(Ok);
	RESULT_STRING_CASE_IMPL(Fail);
	/*RESULT_STRING_CASE_IMPL(ERROR_NULL_PTR);
	RESULT_STRING_CASE_IMPL(ERROR_ZERO_SIZE);
	RESULT_STRING_CASE_IMPL(ERROR_PTR_IS_NOT_NULL);

	RESULT_STRING_CASE_IMPL(ERROR_MEMORY__OUT_OF_MEMORY);
	RESULT_STRING_CASE_IMPL(ERROR_MEMORY__NOT_ENOUGH_BUFFER_MEMORY);

	RESULT_STRING_CASE_IMPL(ERROR_IO__EXCEEDED_MAX_PATH_LENGTH);

	RESULT_STRING_CASE_IMPL(ERROR_IO__FILE_INVALID_FLAGS);
	RESULT_STRING_CASE_IMPL(ERROR_IO__FILE_OPEN_FAILED);
	RESULT_STRING_CASE_IMPL(ERROR_IO__FILE_READ_FAILED);
	RESULT_STRING_CASE_IMPL(ERROR_IO__FILE_WRITE_FAILED);
	RESULT_STRING_CASE_IMPL(ERROR_IO__FILE_SEEK_FAILED);
	RESULT_STRING_CASE_IMPL(ERROR_IO__FILE_FLUSH_FAILED);
	RESULT_STRING_CASE_IMPL(ERROR_IO__FILE_CLOSE_FAILED);*/

	RESULT_STRING_CASE_IMPL(ErrorEcsComponentAlreadyEnabled);
	RESULT_STRING_CASE_IMPL(ErrorEcsComponentNotEnabled);
	RESULT_STRING_CASE_IMPL(ErrorEcsInvalidEntityId);
	RESULT_STRING_CASE_IMPL(ErrorEcsNoEntityAvailable);

	default:;
	}
	return (char*)"INVALID RESULT";
}

static constexpr char* resultInfo(const Result value)
{
#define RESULT_INFO_CASE_IMPL(NAME, MSG)	\
	case eResult##NAME: return (char*)MSG

	switch(value)
	{
	RESULT_INFO_CASE_IMPL(Ok, "Ok.");
	RESULT_INFO_CASE_IMPL(Fail, "Generic fail. \n"
		"Used when a function behaves like boolean.");
	/*RESULT_INFO_CASE_IMPL(ERROR_NULL_PTR, "Null pointer.");
	RESULT_INFO_CASE_IMPL(ERROR_PTR_IS_NOT_NULL, "Pointer is not null. Used when a function need a pointer to be null.");
	RESULT_INFO_CASE_IMPL(ERROR_ZERO_SIZE, "Zero size. Used by functions that need manipulate buffer and the size \n"
		"always need to be greater than zero.");
	RESULT_INFO_CASE_IMPL(ERROR_MEMORY__OUT_OF_MEMORY, 
		"Out of memory. Used when the OS function to allocate fail by lack of memory.");
	RESULT_INFO_CASE_IMPL(ERROR_MEMORY__INVALID_SIZES, "Comparation between size 1 and 2 get fail. \n"
		"Used by reallocate function when old size is equal new size. \n"
		"It doesn't make sense reallocate memory when the sizes are equal.");
	RESULT_INFO_CASE_IMPL(ERROR_MEMORY__NOT_ENOUGH_BUFFER_MEMORY, "Not enough buffer memory."
		"Used in memory manipulation functions when a destination buffer has less memory "
		"than source buffer.");
	RESULT_INFO_CASE_IMPL(ERROR_IO__EXCEEDED_MAX_PATH_LENGTH, "Exceeded max Path length. \n"
		"Used when a Path string is greater than the allowed by the platform.");
	RESULT_INFO_CASE_IMPL(ERROR_IO__FILE_INVALID_FLAGS, "File open invalid flags");
	RESULT_INFO_CASE_IMPL(ERROR_IO__FILE_OPEN_FAILED, "File open failed");
	RESULT_INFO_CASE_IMPL(ERROR_IO__FILE_READ_FAILED, "File read failed");
	RESULT_INFO_CASE_IMPL(ERROR_IO__FILE_WRITE_FAILED, "File write failed");
	RESULT_INFO_CASE_IMPL(ERROR_IO__FILE_SEEK_FAILED, "File seek failed");
	RESULT_INFO_CASE_IMPL(ERROR_IO__FILE_FLUSH_FAILED, "File flush failed");
	RESULT_INFO_CASE_IMPL(ERROR_IO__FILE_CLOSE_FAILED, "File close failed");*/
	default:;
	}
	return (char*)"INVALID RESULT";
}

#endif
