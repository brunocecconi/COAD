
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
#include "Editor/imgui.h"

#if PLATFORM_WINDOWS
#include <directx/d3d12.h>
#include "Editor/Windows/imgui_impl_win32.h"
#include "Editor/Windows/imgui_impl_dx12.h"
#endif

namespace Engine
{

class Manager;
class Window;

} // namespace Engine

namespace Render
{
class PlatformManager;
}

namespace Editor
{

class Manager: public ManagerThread<Manager>
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(Manager);

public:
	using base_t = ManagerThread<Manager>;
	friend class base_t;

private:
	Manager();

public:
	~Manager();

protected:
	void Initialize(const Engine::Window& Window, RESULT_PARAM_DEFINE);
	void RunInternal(RESULT_PARAM_DEFINE);
	void RunExternal(RESULT_PARAM_DEFINE);
	void Finalize(RESULT_PARAM_DEFINE);

private:
	friend class Engine::Manager;
	friend class Render::PlatformManager;

#if PLATFORM_WINDOWS
	ID3D12DescriptorHeap* mSrvDescHeap{};
	D3D12_CPU_DESCRIPTOR_HANDLE mCpuSrvDescHeap{};
	D3D12_GPU_DESCRIPTOR_HANDLE mGpuSrvDescHeap{};
#endif
	bool mFileSelected{};
};

} // namespace Editor

#endif

#endif
