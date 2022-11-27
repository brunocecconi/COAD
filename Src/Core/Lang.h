
/** \file Lang.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_LANG_H
#define CORE_LANG_H

#include "Core/Platform.h"

#include <EASTL/vector.h>
#include <EASTL/set.h>

#define NO_BODY

template<typename T>
concept is_coad_class =
	eastl::is_same_v<void, eastl::void_t<typename T::CoadClass>> && eastl::is_same_v<typename T::this_t, T>;

#define CLASS_BODY_ONLY_HEADER(CLASS_NAME)                                                                             \
public:                                                                                                                \
	struct CoadClass                                                                                             \
	{                                                                                                                  \
	};                                                                                                                 \
	using this_t = CLASS_NAME;

#define CLASS_BODY_ONLY_HEADER_NAMED(CLASS_NAME, CUSTOM_NAME)                                                          \
public:                                                                                                                \
	struct CoadClass                                                                                             \
	{                                                                                                                  \
	};                                                                                                                 \
	using this_t = CLASS_NAME;

#define CLASS_BODY_CUSTOM(CLASS_NAME, body_move, body_copy)                                                            \
	CLASS_BODY_ONLY_HEADER(CLASS_NAME)                                                                                 \
	body_move body_copy

#define CLASS_BODY(CLASS_NAME) CLASS_BODY_CUSTOM(CLASS_NAME, CLASS_MOVEABLE(CLASS_NAME), CLASS_COPYABLE(CLASS_NAME))

#define CLASS_BODY_NO_HEADER(CLASS_NAME)                                                                               \
public:                                                                                                                \
	CLASS_MOVEABLE(CLASS_NAME)                                                                                         \
	CLASS_COPYABLE(CLASS_NAME)

#define CLASS_BODY_NON_MOVEABLE_COPYABLE(CLASS_NAME) CLASS_BODY_CUSTOM(CLASS_NAME, NO_BODY, NO_BODY)

#define CLASS_BODY_NON_MOVEABLE(CLASS_NAME)                                                                            \
	CLASS_BODY_CUSTOM(CLASS_NAME, CLASS_NON_MOVEABLE(CLASS_NAME), CLASS_COPYABLE(CLASS_NAME))

#define CLASS_BODY_NON_COPYABLE(CLASS_NAME)                                                                            \
	CLASS_BODY_CUSTOM(CLASS_NAME, CLASS_MOVEABLE(CLASS_NAME), CLASS_NON_COPYABLE(CLASS_NAME))

#define CLASS_MOVEABLE(CLASS_NAME)                                                                                     \
	CLASS_NAME(CLASS_NAME&&) noexcept			 = default;                                                            \
	CLASS_NAME& operator=(CLASS_NAME&&) noexcept = default;

#define CLASS_NON_MOVEABLE(CLASS_NAME)                                                                                 \
	CLASS_NAME(CLASS_NAME&&) noexcept			 = delete;                                                             \
	CLASS_NAME& operator=(CLASS_NAME&&) noexcept = delete;

#define CLASS_COPYABLE(CLASS_NAME)                                                                                     \
	CLASS_NAME(const CLASS_NAME&) noexcept			  = default;                                                       \
	CLASS_NAME& operator=(const CLASS_NAME&) noexcept = default;

#define CLASS_NON_COPYABLE(CLASS_NAME)                                                                                 \
	CLASS_NAME(const CLASS_NAME&) noexcept			  = delete;                                                        \
	CLASS_NAME& operator=(const CLASS_NAME&) noexcept = delete;

#define CLASS_VALIDATION(CLASS_NAME)                                                                                   \
	static_assert(is_coad_class<CLASS_NAME>,                                                                           \
				  "Invalid COAD class. Hint: You need to put CLASS_BODY() macro inside the class.");                   \
	static_assert(!eastl::is_polymorphic_v<CLASS_NAME>, "The class cannot be polymorphic.");                           \
	static_assert(!eastl::is_trivially_constructible_v<CLASS_NAME>, "The class cannot be trivially constructible.");   \
	static_assert(!eastl::is_trivially_destructible_v<CLASS_NAME>, "The class cannot be trivially destructible.");
/*\
META_TYPE_DEFINE(CLASS_NAME);	\
META_TYPE_DEFINE(eastl::vector<CLASS_NAME>);	\
META_TYPE_DEFINE(eastl::set<CLASS_NAME>);*/

#endif
