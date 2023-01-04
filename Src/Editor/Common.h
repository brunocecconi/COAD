
/** \file Editor/Common.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef EDITOR_COMMON_H
#define EDITOR_COMMON_H

#ifndef RELEASE
#ifndef EDITOR
#define EDITOR 1
#endif
#else
#undef EDITOR
#define EDITOR 0
#endif

#endif
