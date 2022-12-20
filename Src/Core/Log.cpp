
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

uint64_t gVerbosity{};

uint64_t Verbosity()
{
	return gVerbosity;
}

void SetVerbosity(const uint64_t Value)
{
	gVerbosity = Value;
}

}

#endif
