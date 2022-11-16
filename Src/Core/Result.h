

#ifndef RESULT_H
#define RESULT_H

#include "Core/BaseTypes.h"

#if _MSC_VER
#pragma warning (disable : 4100)
#endif

#if defined(RELEASE) 
#undef USE_RESULT
#define USE_RESULT 0
#endif

#if USE_RESULT

#define RESULT							Result
#define RESULT_VAR(NAME)				RESULT* NAME = nullptr
#define RESULT_SET_VAR(NAME, VALUE)		NAME = (Result*)(VALUE)
#define RESULT_GET_VAR(NAME)			NAME
#define RESULT_SET(VALUE)				if(result) RESULT_SET_VAR(*result, VALUE)
#define RESULT_GET()					RESULT_GET_VAR(*result)
#define RESULT_OK_PTR					(Result*)(eResultOk)
#define RESULT_OK()						RESULT_SET(eResultOk)
#define RESULT_ERROR(VALUE, ...)		RESULT_SET(VALUE); return __VA_ARGS__
#define RESULT_ARG_OPT					,RESULT** result = nullptr
#define RESULT_ARG						,RESULT** result
#define RESULT_ARG_PASS					,result
#define RESULT_ARG_SINGLE_OPT			RESULT** result = nullptr
#define RESULT_ARG_SINGLE				RESULT** result

#define ENSURE_RESULT_NL(RESULT, ...)	\
if((result ? *result : RESULT_OK_PTR) != RESULT_OK_PTR) { return __VA_ARGS__; }

#define ENSURE_RESULT(RESULT, ...)	\
if((result ? *result : RESULT_OK_PTR) != RESULT_OK_PTR) { printf(CONSOLE_COLOR_LIGHT_YELLOW CONSOLE_COLOR_BOLD	\
	"ENSURE RESULT FAILED '" #RESULT \
	"' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT, __FILE__, __LINE__, \
	resultString(RESULT), resultInfo(RESULT)); return __VA_ARGS__; }

#define ENSURE_LAST_RESULT_NL(...)	\
if((result ? (*result != RESULT_OK_PTR) : false)) { return __VA_ARGS__; }

#define ENSURE_LAST_RESULT(...)	\
if((result ? (*result != RESULT_OK_PTR) : false)) { printf(CONSOLE_COLOR_LIGHT_YELLOW CONSOLE_COLOR_BOLD	\
	"ENSURE LAST RESULT FAILED " \
	" at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT, __FILE__, __LINE__, \
	resultString((Result)(u64)result), resultInfo((Result)(u64)result)); return __VA_ARGS__; }

#define ENFORCE_RESULT_NL(RESULT)	\
if((RESULT) != RESULT_OK_PTR) { abort(); }

#define ENFORCE_RESULT(RESULT)	\
if((RESULT) != RESULT_OK_PTR) { printf(CONSOLE_COLOR_LIGHT_RED LOG_COLOR_BOLD	\
	"ENFORCE RESULT FAILED '" #RESULT \
	"' at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT, __FILE__, __LINE__, \
	resultString(RESULT), resultInfo(RESULT)); abort(); }

#define ENFORCE_LAST_RESULT_NL()	\
if((result ? (*result != RESULT_OK_PTR) : false)) { abort(); }

#define ENFORCE_LAST_RESULT()	\
if((result ? (*result != RESULT_OK_PTR) : false)) { printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD	\
	"ENFORCE LAST RESULT FAILED " \
	" at '%s':'%d'.\nRESULT STRING: %s\nRESULT INFO: %s\n" CONSOLE_COLOR_DEFAULT, __FILE__, __LINE__, \
	resultString((Result)(u64)result), resultInfo((Result)(u64)result)); abort(); }

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
enum Result : u64
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

#else

#define RESULT
#define RESULT_VAR(NAME)
#define RESULT_SET_VAR(NAME, VALUE)
#define RESULT_GET_VAR(NAME)
#define RESULT_SET(VALUE)
#define RESULT_GET()
#define RESULT_OK_PTR
#define RESULT_OK()
#define RESULT_ERROR(VALUE, ...)
#define RESULT_ARG_OPT
#define RESULT_ARG
#define RESULT_ARG_PASS
#define RESULT_ARG_SINGLE_OPT
#define RESULT_ARG_SINGLE

#define ENSURE_RESULT_NL(RESULT, ...)
#define ENSURE_RESULT(RESULT, ...)
#define ENSURE_LAST_RESULT(...)
#define ENSURE_LAST_RESULT_NL(...)
#define ENFORCE_RESULT_NL(RESULT)
#define ENFORCE_RESULT(RESULT)
#define ENFORCE_LAST_RESULT_NL(RESULT)
#define ENFORCE_LAST_RESULT(RESULT)

#define RESULT_ENSURE_CALL(FUNCTION, ...)		FUNCTION
#define RESULT_ENSURE_CALL_NL(FUNCTION, ...)	FUNCTION
#define RESULT_ENFORCE_CALL(FUNCTION, ...)		FUNCTION
#define RESULT_ENFORCE_CALL_NL(FUNCTION, ...)	FUNCTION

#endif

#endif
