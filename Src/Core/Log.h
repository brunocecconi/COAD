
/** @file Log.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_LOG_H
#define CORE_LOG_H

#include "Core/BaseTypes.h"
#include <ctime>

#define LOG_DEFINE(NAME)                                                                                               \
	namespace Log                                                                                                      \
	{                                                                                                                  \
	struct NAME                                                                                                        \
	{                                                                                                                  \
	};                                                                                                                 \
	}

#define LOG_CATEGORY_DEFINE(VERBOSITY_SYMBOL, NAME, COLORS, ...)                                                       \
	namespace Log                                                                                                      \
	{                                                                                                                  \
	struct Category##NAME                                                                                              \
	{                                                                                                                  \
		static constexpr auto VERBOSITY = VERBOSITY_SYMBOL;                                                            \
		static constexpr auto VALUE		= " - " COLORS "[" #NAME "]: ";                                                \
		static void			  Apply()                                                                                  \
		{                                                                                                              \
			__VA_ARGS__                                                                                                \
		}                                                                                                              \
	};                                                                                                                 \
	}

namespace Log
{

Uint64 Verbosity();
void   SetVerbosity(Uint64 value);

} // namespace Log

#define LOG(CATEGORY, MSG, ...)                                                                                        \
	if (Log::Category##CATEGORY::VERBOSITY <= Log::Verbosity())                                                        \
	{                                                                                                                  \
		time_t l_time = time(NULL);                                                                                    \
		auto   l_tm	  = localtime(&l_time);                                                                            \
		char   l_time_string[64];                                                                                      \
		strftime(l_time_string, sizeof(l_time_string), "%c", l_tm);                                                    \
		printf(CONSOLE_COLOR_WHITE CONSOLE_COLOR_BOLD "[%s]" CONSOLE_COLOR_DEFAULT, l_time_string);                    \
		printf("%s" MSG "%s\n", Log::Category##CATEGORY::VALUE, __VA_ARGS__, CONSOLE_COLOR_DEFAULT);                   \
		Log::Category##CATEGORY::Apply();                                                                              \
	}

#define LOG_SET_VERBOSITY(CATEGORY) Log::SetVerbosity(Log::Category##CATEGORY::VERBOSITY)

LOG_CATEGORY_DEFINE(0, error, CONSOLE_COLOR_LIGHT_RED)
LOG_CATEGORY_DEFINE(1, warning, CONSOLE_COLOR_LIGHT_YELLOW)
LOG_CATEGORY_DEFINE(0, fatal, CONSOLE_COLOR_RED, abort();)
LOG_CATEGORY_DEFINE(3, info, CONSOLE_COLOR_DEFAULT)
LOG_CATEGORY_DEFINE(4, verbose, CONSOLE_COLOR_LIGHT_CYAN)

LOG_DEFINE(core)

#endif
