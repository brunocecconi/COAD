
#include "Core/Log.h"

u64 g_verbosity{};

u64 logVerbosity()
{
	return g_verbosity;
}

void setLogVerbosity(const u64 value)
{
	g_verbosity = value;
}
