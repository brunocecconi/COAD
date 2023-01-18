
/** \file Editor/Manager.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef EDITOR_MANAGER_H
#define EDITOR_MANAGER_H

#include "Editor/Common.h"

#if EDITOR

#include "Core/Manager.h"
#include <imgui.h>

#if PLATFORM_WINDOWS
#include <imgui_impl_win32.h>

#if OPENGL_ENABLED
#include <imgui_impl_opengl3.h>
#elif VULKAN_ENABLED
#include <imgui_impl_vulkan.h>
#endif
#endif

namespace Engine
{

class Manager;
class Window;

} // namespace Engine

namespace Render::Platform
{
class OpenglManager;
}

namespace Editor
{

class Manager: public ManagerThread<Manager>
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(Manager);

public:
	using base_t = ManagerThread<Manager>;
	friend class ManagerThread<Manager>;

private:
	Manager();

public:
	~Manager();

protected:
	void Initialize(const Engine::Window& Window, RESULT_PARAM_DEFINE);
	void PreRunLoop(RESULT_PARAM_DEFINE);
	void RunInternal(RESULT_PARAM_DEFINE);
	void RunExternal(RESULT_PARAM_DEFINE);
	void Finalize(RESULT_PARAM_DEFINE);

private:
	friend class Engine::Manager;
	friend class Render::Platform::OpenglManager;

	bool mFileSelected{};
};

INLINE Manager& Instance()
{
	return Manager::Instance();
}

} // namespace Editor

#endif

#endif
