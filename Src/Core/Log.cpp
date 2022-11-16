
/** \file Log.cpp
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#include "Core/Log.h"

Uint64 g_verbosity{};

Uint64 logVerbosity()
{
	return g_verbosity;
}

void setLogVerbosity(const Uint64 value)
{
	g_verbosity = value;
}
