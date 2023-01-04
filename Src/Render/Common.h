
#ifndef RENDER_COMMON_H
#define RENDER_COMMON_H

#include "Core/Common.h"

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
#include <directx/d3dx12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
using namespace Microsoft::WRL;
#endif

#endif

