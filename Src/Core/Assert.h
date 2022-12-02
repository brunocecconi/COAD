
/** \file Assert.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef ASSERT_H
#define ASSERT_H

#include "Core/Log.h"

#include <cassert>

#define ENSURE_NOLOG(EXP, ...)                                                                                         \
	if (!(EXP))                                                                                                        \
	{                                                                                                                  \
		return __VA_ARGS__;                                                                                           \
	}

#define ENSURE(EXP, ...)                                                                                               \
	if (!(EXP))                                                                                                        \
	{                                                                                                                  \
		printf(LOG_COLOR_LIGHT_YELLOW LOG_COLOR_BOLD "ENSURE FAILED '" #EXP "' at '%s':'%d'\n" LOG_COLOR_DEFAULT,      \
			   __FILE__, __LINE__);                                                                                    \
		return __VA_ARGS__;                                                                                           \
	}

#define ENFORCE_NOLOG(EXP)                                                                                             \
	if (!(EXP))                                                                                                        \
	{                                                                                                                  \
		exit(-1);                                                                                                      \
	}

#define ENFORCE(EXP)                                                                                                   \
	if (!(EXP))                                                                                                        \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD "ENFORCE FAILED '" #EXP                                      \
														  "' at '%s':'%d'\n" CONSOLE_COLOR_DEFAULT,                    \
			   __FILE__, __LINE__);                                                                                    \
		exit(-1);                                                                                                      \
	}

#define ENFORCE_MSG(EXP, MSG, ...)                                                                                     \
	if (!(EXP))                                                                                                        \
	{                                                                                                                  \
		printf(CONSOLE_COLOR_LIGHT_RED CONSOLE_COLOR_BOLD "ENFORCE FAILED '" #EXP "' at '%s':'%d'. Message: ",         \
			   __FILE__, __LINE__);                                                                                    \
		printf(MSG CONSOLE_COLOR_DEFAULT, __VA_ARGS__);                                                                \
		exit(-1);                                                                                                      \
	}

#ifndef RELEASE
#define ASSERT_NOLOG(EXP)		  ENFORCE_NOLOG(EXP)
#define ASSERT(EXP)				  ENFORCE(EXP)
#define ASSERT_MSG(EXP, MSG, ...) ENFORCE_MSG(EXP, MSG, __VA_ARGS__)
#define ASSERT_FAIL()			  ASSERT(false)
#define ASSERT_FAIL_MSG(MSG, ...) ASSERT_MSG(false, MSG, __VA_ARGS__)
#else
#define ASSERT_NOLOG(EXP)
#define ASSERT(EXP)
#define ASSERT_MSG(EXP, MSG, ...)
#define ASSERT_FAIL()
#define ASSERT_FAIL_MSG(MSG, ...)
#endif

#endif
