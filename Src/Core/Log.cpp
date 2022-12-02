
/** \file Log.cpp
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef RELEASE

#include "Core/Log.h"

namespace Log
{

Uint64 g_verbosity{};

Uint64 Verbosity()
{
	return g_verbosity;
}

void SetVerbosity(const Uint64 value)
{
	g_verbosity = value;
}

}

#endif
