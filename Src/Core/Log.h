
#ifndef CORE_LOG_H
#define CORE_LOG_H

#include "Core/BaseTypes.h"
#include <ctime>

#define LOG_DEFINE(NAME)	\
struct Log##NAME{};

#define LOG_CATEGORY_DEFINE(VERBOSITY, NAME, COLORS, ...)	\
struct LogCategory##NAME	\
{	\
	static constexpr auto verbosity = VERBOSITY;	\
	static constexpr auto value = " - " COLORS "[" #NAME "]: ";	\
	static void apply(){ __VA_ARGS__ }	\
};



u64 logVerbosity();
void setLogVerbosity(u64 value);

#define LOG(CATEGORY, MSG, ...)	\
if(LogCategory##NAME::verbosity <= logVerbosity())	\
{	\
	time_t l_time = time(NULL);	\
    auto l_tm = localtime(&l_time);	\
    char l_time_string[64];	\
    strftime(l_time_string, sizeof(l_time_string), "%c", l_tm);	\
    printf(CONSOLE_COLOR_WHITE CONSOLE_COLOR_BOLD "[%s]" CONSOLE_COLOR_DEFAULT, l_time_string);	\
	printf("%s" MSG "%s\n", LogCategory##NAME::value, __VA_ARGS__, CONSOLE_COLOR_DEFAULT);	\
	LogCategory##NAME::apply();	\
}

#define LOG_SET_VERBOSITY(CATEGORY)	\
setLogVerbosity(LogCategory##CATEGORY::verbosity)

LOG_CATEGORY_DEFINE(0, error, CONSOLE_COLOR_LIGHT_RED)
LOG_CATEGORY_DEFINE(1, warning, CONSOLE_COLOR_LIGHT_YELLOW)
LOG_CATEGORY_DEFINE(0, fatal, CONSOLE_COLOR_RED, abort(); )
LOG_CATEGORY_DEFINE(3, info, CONSOLE_COLOR_DEFAULT)
LOG_CATEGORY_DEFINE(4, verbose, CONSOLE_COLOR_LIGHT_CYAN)

LOG_DEFINE(core)

#endif
