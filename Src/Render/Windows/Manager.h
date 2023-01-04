
#ifdef RENDER_INCLUDE_PLATFORM_MANAGER

/** \file PlatformManager.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

namespace Render
{

/**
 * @brief Platform manager class.
 *
 * Designed to dx12 in windows.
 *
 */
class PlatformManager
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(PlatformManager);

private:
	PlatformManager();

public:
	~PlatformManager();

private:
	void Initialize(RESULT_PARAM_DEFINE);
	void Update(RESULT_PARAM_DEFINE);
	void Finalize(RESULT_PARAM_DEFINE);
	void Resize(glm::uvec2 NewSize, RESULT_PARAM_DEFINE);

private:
	ComPtr<ID3D12Resource> PrepareFrameUpdate(RESULT_PARAM_DEFINE);
	void				   Present(ComPtr<ID3D12Resource> BackBuffer, RESULT_PARAM_DEFINE);

private:
	void ToggleFullscreen(RESULT_PARAM_DEFINE);

private:
	NODISCARD static bool CheckTearingSupport(RESULT_PARAM_DEFINE);
	NODISCARD static bool CheckRaytracingSupport(ComPtr<ID3D12Device> Device, RESULT_PARAM_DEFINE);

private:
	NODISCARD static ComPtr<IDXGIAdapter4>		  GetAdapter(bool UseWarp, RESULT_PARAM_DEFINE);
	NODISCARD static ComPtr<ID3D12Device>		  CreateDevice(ComPtr<IDXGIAdapter4> Adapter, RESULT_PARAM_DEFINE);
	NODISCARD static ComPtr<ID3D12CommandQueue>	  CreateCommandQueue(ComPtr<ID3D12Device>	 Device,
																	 D3D12_COMMAND_LIST_TYPE Type, RESULT_PARAM_DEFINE);
	NODISCARD static ComPtr<IDXGISwapChain4>	  CreateSwapchain(HWND Hwnd, ComPtr<ID3D12CommandQueue> CommandQueue,
																  uint32_t Width, uint32_t Height, uint32_t BufferCount,
																  RESULT_PARAM_DEFINE);
	NODISCARD static ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device>		  Device,
																	   D3D12_DESCRIPTOR_HEAP_TYPE Type,
																	   uint32_t NumDescriptors, RESULT_PARAM_DEFINE);
	void UpdateRenderTargetViews(ComPtr<ID3D12Device> Device, ComPtr<IDXGISwapChain4> Swapchain,
								 ComPtr<ID3D12DescriptorHeap> DescriptorHeap, RESULT_PARAM_DEFINE);
	NODISCARD static ComPtr<ID3D12CommandAllocator>	   CreateCommandAllocator(ComPtr<ID3D12Device>	  Device,
																			  D3D12_COMMAND_LIST_TYPE Type,
																			  RESULT_PARAM_DEFINE);
	NODISCARD static ComPtr<ID3D12GraphicsCommandList> CreateCommandList(
		ComPtr<ID3D12Device> Device, ComPtr<ID3D12CommandAllocator> CommandAllocator, D3D12_COMMAND_LIST_TYPE Type,
		RESULT_PARAM_DEFINE);
	NODISCARD static ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device> Device, RESULT_PARAM_DEFINE);
	NODISCARD static HANDLE				 CreateFenceEvent(RESULT_PARAM_DEFINE);
	NODISCARD static uint64_t			 SignalFence(ComPtr<ID3D12CommandQueue> CommandQueue, ComPtr<ID3D12Fence> Fence,
													 uint64_t& FenceValue, RESULT_PARAM_DEFINE);
	static void WaitForFenceValue(ComPtr<ID3D12Fence> Fence, uint64_t FenceValue, HANDLE FenceEvent,
								  uint64_t MsDuration = UINT64_MAX, RESULT_PARAM_DEFINE);
	static void Flush(ComPtr<ID3D12CommandQueue> CommandQueue, ComPtr<ID3D12Fence> Fence, uint64_t& FenceValue,
					  HANDLE FenceEvent, RESULT_PARAM_DEFINE);
	void		Flush(RESULT_PARAM_DEFINE);
	void		WaitFrameFences(RESULT_PARAM_DEFINE);

private:
	friend class Manager;
	friend class Engine::Window;
	Engine::Window&		  mWindow;
	static constexpr auto NUM_FRAMES	  = 3;
	bool				  mUseWarp		  = false;

private:
#if EDITOR
	friend class Editor::Manager;
#endif

	// Core objects
	ComPtr<ID3D12Device>			  mDevice;
	ComPtr<ID3D12CommandQueue>		  mCommandQueue;
	ComPtr<IDXGISwapChain4>			  mSwapchain;
	ComPtr<ID3D12Resource>			  mBackBuffers[NUM_FRAMES];
	ComPtr<ID3D12GraphicsCommandList> mCommandList;
	ComPtr<ID3D12CommandAllocator>	  mCommandAllocators[NUM_FRAMES];
	ComPtr<ID3D12DescriptorHeap>	  mRtvDescriptorHeap;
	uint32_t						  mRtvDescriptorSize{};
	D3D12_CPU_DESCRIPTOR_HANDLE		  mRtvCpuDescriptorHandle[NUM_FRAMES];
	D3D12_GPU_DESCRIPTOR_HANDLE		  mRtvGpuDescriptorHandle{};
	uint32_t						  mCurrentBackBufferIndex{};

	// Sync objects
	ComPtr<ID3D12Fence> mFence;
	uint64_t			mFenceValue{};
	uint64_t			mFrameFenceValues[NUM_FRAMES] = {};
	HANDLE				mFenceEvent;

	// Swap chain present control
	bool mVsync			   = true;
	bool mTearingSupported = false;
	bool mFullscreen	   = false;
};

INLINE PlatformManager::PlatformManager() : mWindow{Engine::Manager::Instance().GetWindow()}
{
}

INLINE PlatformManager::~PlatformManager()
{
}

INLINE void PlatformManager::Initialize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

#ifdef DEBUG
	ComPtr<ID3D12Debug> lDebugInterface;
	const auto			lHresult = D3D12GetDebugInterface(IID_PPV_ARGS(&lDebugInterface));
	RESULT_CONDITION_ENSURE(lHresult == S_OK, PlatformRenderFailedToInitialize);
	lDebugInterface->EnableDebugLayer();
#endif

	const auto lWindowSize = mWindow.GetSize();

	RESULT_ENSURE_CALL(const ComPtr<IDXGIAdapter4> lAdapter = GetAdapter(false, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mDevice = CreateDevice(lAdapter, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mCommandQueue = CreateCommandQueue(mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mSwapchain = CreateSwapchain(mWindow.GetHandle(), mCommandQueue, lWindowSize.x, lWindowSize.y,
													NUM_FRAMES, RESULT_ARG_PASS));

	mCurrentBackBufferIndex = mSwapchain->GetCurrentBackBufferIndex();

	// RESULT_ENSURE_CALL(mRtvDescriptorHeap =
	// CreateDescriptorHeap(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NUM_FRAMES, RESULT_ARG_PASS));

	RESULT_ENSURE_CALL(mRtvDescriptorHeap =
						   CreateDescriptorHeap(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NUM_FRAMES, RESULT_ARG_PASS));

	mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_CPU_DESCRIPTOR_HANDLE lRtvCpuDescriptorHandle = mRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (uint32_t i = 0; i < NUM_FRAMES; ++i)
	{
		mRtvCpuDescriptorHandle[i] = lRtvCpuDescriptorHandle;
		lRtvCpuDescriptorHandle.ptr += mRtvDescriptorSize;
	}

	// mGpuDescriptorHandle = mRtvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	RESULT_ENSURE_CALL(UpdateRenderTargetViews(mDevice, mSwapchain, mRtvDescriptorHeap, RESULT_ARG_PASS));

	for (int32_t lI = 0; lI < NUM_FRAMES; ++lI)
	{
		RESULT_ENSURE_CALL(mCommandAllocators[lI] =
							   CreateCommandAllocator(mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, RESULT_ARG_PASS));
	}
	RESULT_ENSURE_CALL(mCommandList = CreateCommandList(mDevice, mCommandAllocators[mCurrentBackBufferIndex],
														D3D12_COMMAND_LIST_TYPE_DIRECT, RESULT_ARG_PASS));

	RESULT_ENSURE_CALL(mFence = CreateFence(mDevice, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mFenceEvent = CreateFenceEvent(RESULT_ARG_PASS));

	RESULT_ENSURE_CALL(const bool lAllowRaytracing = CheckRaytracingSupport(mDevice, RESULT_ARG_PASS));
	(void)lAllowRaytracing;

	RESULT_OK();
}

INLINE void PlatformManager::Update(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

#if EDITOR
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Editor::Manager::Instance().RunExternal(RESULT_ARG_PASS);
#endif

	RESULT_ENSURE_CALL(const auto lBackBuffer = PrepareFrameUpdate(RESULT_ARG_PASS));

#if EDITOR
	ImGui::Render();
	mCommandList->SetDescriptorHeaps(1, &Editor::Manager::Instance().mSrvDescHeap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());
#endif

	RESULT_ENSURE_CALL(Present(lBackBuffer, RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE void PlatformManager::Finalize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(Flush(mCommandQueue, mFence, mFenceValue, mFenceEvent, RESULT_ARG_PASS));
	::CloseHandle(mFenceEvent);
	RESULT_OK();
}

INLINE void PlatformManager::Resize(const glm::uvec2 NewSize, RESULT_PARAM_IMPL)
{
	RESULT_CONDITION_ENSURE(Manager::Instance().IsInThread(), CurrentThreadIsNotTheRequiredOne);

	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(NewSize.x > 0 && NewSize.y > 0, ZeroSize);

	if (const auto lFromSize = mWindow.GetPreviousSize(); mWindow.GetSize() != lFromSize)
	{
		RESULT_ENSURE_CALL(Flush(mCommandQueue, mFence, mFenceValue, mFenceEvent, RESULT_ARG_PASS));

		for (int lI = 0; lI < NUM_FRAMES; ++lI)
		{
			// Any references to the back buffers must be released
			// before the swap chain can be resized.
			mBackBuffers[lI].Reset();
			mFrameFenceValues[lI] = mFrameFenceValues[mCurrentBackBufferIndex];
		}

		DXGI_SWAP_CHAIN_DESC lSwapChainDesc = {};
		RESULT_CONDITION_ENSURE(mSwapchain->GetDesc(&lSwapChainDesc) == S_OK, Dx12FailedToGetSwapChainDescription);
		const HRESULT lResult = mSwapchain->ResizeBuffers(lSwapChainDesc.BufferCount, NewSize.x, NewSize.y,
														  lSwapChainDesc.BufferDesc.Format, lSwapChainDesc.Flags);
		RESULT_CONDITION_ENSURE(lResult == S_OK, Dx12FailedToResizeSwapChain);

		mCurrentBackBufferIndex = mSwapchain->GetCurrentBackBufferIndex();

		RESULT_ENSURE_CALL(UpdateRenderTargetViews(mDevice, mSwapchain, mRtvDescriptorHeap, RESULT_ARG_PASS));

		LOGC(Info, PlatformRender, "Resized window render target from [%u, %u] to [%u, %u].", lFromSize.x, lFromSize.y,
			NewSize.x, NewSize.y);
	}

	RESULT_OK();
}

ComPtr<ID3D12Resource> PlatformManager::PrepareFrameUpdate(RESULT_PARAM_IMPL)
{
	ManagerWait<Render::Manager>{RESULT_ARG_PASS};

	RESULT_ENSURE_LAST({});
	RESULT_ENSURE_CALL(Flush(RESULT_ARG_PASS), {});
	const auto lCommandAllocator = mCommandAllocators[mCurrentBackBufferIndex];
	const auto lBackBuffer		 = mBackBuffers[mCurrentBackBufferIndex];

	lCommandAllocator->Reset();
	mCommandList->Reset(lCommandAllocator.Get(), nullptr);

	// Clear the render target
	{
		const CD3DX12_RESOURCE_BARRIER lBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			lBackBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mCommandList->ResourceBarrier(1, &lBarrier);

		constexpr FLOAT						lClearColor[] = {0.125f, 0.125f, 0.5f, 1.f};
		const CD3DX12_CPU_DESCRIPTOR_HANDLE lRtv{mRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
												 static_cast<INT>(mCurrentBackBufferIndex), mRtvDescriptorSize};

		mCommandList->ClearRenderTargetView(lRtv, lClearColor, 0, nullptr);
		mCommandList->OMSetRenderTargets(1, &mRtvCpuDescriptorHandle[mCurrentBackBufferIndex], FALSE, NULL);
	}
	RESULT_OK();
	return lBackBuffer;
}

INLINE void PlatformManager::Present(const ComPtr<ID3D12Resource> BackBuffer, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	// Present procedure
	{
		// Set barrier
		const CD3DX12_RESOURCE_BARRIER lBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			BackBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		mCommandList->ResourceBarrier(1, &lBarrier);
		RESULT_CONDITION_ENSURE(mCommandList->Close() == S_OK, Dx12FailedToCloseCommandList);

		// Pre present
		ID3D12CommandList* lCommandLists[] = {mCommandList.Get()};
		mCommandQueue->ExecuteCommandLists(_countof(lCommandLists), lCommandLists);

		// Applies signal value to command queue
		RESULT_ENSURE_CALL(mFrameFenceValues[mCurrentBackBufferIndex] =
							   SignalFence(mCommandQueue, mFence, mFenceValue, RESULT_ARG_PASS));

		// Present
		const UINT lSyncInterval = mVsync ? 1 : 0;
		const UINT lPresentFlags = mTearingSupported && !mVsync ? DXGI_PRESENT_ALLOW_TEARING : 0;
		RESULT_CONDITION_ENSURE(mSwapchain->Present(lSyncInterval, lPresentFlags) == S_OK, Dx12FailedToPresent);

		// Post present
		// Updates current back buffer index
		// Waits for fence value be signaled in command queue (cpu stall)
		mCurrentBackBufferIndex = mSwapchain->GetCurrentBackBufferIndex();
		RESULT_ENSURE_CALL(WaitForFenceValue(mFence, mFrameFenceValues[mCurrentBackBufferIndex], mFenceEvent,
											 UINT64_MAX, RESULT_ARG_PASS));
	}
	RESULT_OK();
}

INLINE void PlatformManager::ToggleFullscreen(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(Resize(mWindow.GetSize(), RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE ComPtr<IDXGIAdapter4> PlatformManager::GetAdapter(const bool UseWarp, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST({});

	ComPtr<IDXGIFactory4> lDxgiFactory;
	UINT				  lCreateFactoryFlags = 0;
#if defined(_DEBUG)
	lCreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	RESULT_CONDITION_ENSURE(CreateDXGIFactory2(lCreateFactoryFlags, IID_PPV_ARGS(&lDxgiFactory)) == S_OK,
							Dx12FailedToCreateFactory, {});

	ComPtr<IDXGIAdapter1> lDxgiAdapter1;
	ComPtr<IDXGIAdapter4> lDxgiAdapter4;

	if (UseWarp)
	{
		RESULT_CONDITION_ENSURE(lDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&lDxgiAdapter1)) == S_OK,
								Dx12FailedToGetAdapter, {});
		RESULT_CONDITION_ENSURE(lDxgiAdapter1.As(&lDxgiAdapter4) == S_OK, Dx12FailedToGetAdapter, {});
	}
	else
	{
		SIZE_T lMaxDedicatedVideoMemory = 0;
		for (UINT lI = 0; lDxgiFactory->EnumAdapters1(lI, &lDxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++lI)
		{
			DXGI_ADAPTER_DESC1 lDxgiAdapterDesc1;
			lDxgiAdapter1->GetDesc1(&lDxgiAdapterDesc1);

			// Check to see if the adapter can create a D3D12 device without actually
			// creating it. The adapter with the largest dedicated video memory
			// is favored.
			if ((lDxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
				SUCCEEDED(
					D3D12CreateDevice(lDxgiAdapter1.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)) &&
				lDxgiAdapterDesc1.DedicatedVideoMemory > lMaxDedicatedVideoMemory)
			{
				lMaxDedicatedVideoMemory = lDxgiAdapterDesc1.DedicatedVideoMemory;
				RESULT_CONDITION_ENSURE(lDxgiAdapter1.As(&lDxgiAdapter4) == S_OK, Dx12FailedToGetAdapter, {});
			}
		}
	}

	RESULT_OK();
	return lDxgiAdapter4;
}

INLINE ComPtr<ID3D12Device> PlatformManager::CreateDevice(const ComPtr<IDXGIAdapter4> Adapter, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST({});
	RESULT_CONDITION_ENSURE(Adapter, NullPtr, {});

	ComPtr<ID3D12Device> lDevice;
	RESULT_CONDITION_ENSURE(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&lDevice)) == S_OK,
							Dx12FailedToCreateDevice, {});

#ifdef DEBUG
	ComPtr<ID3D12InfoQueue> lInfoQueue;
	if (SUCCEEDED(lDevice.As(&lInfoQueue)))
	{
		lInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		lInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		lInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
	}

	// Suppress messages based on their severity level
	D3D12_MESSAGE_SEVERITY lSeverities[] = {D3D12_MESSAGE_SEVERITY_INFO};

	// Suppress individual messages by their ID
	D3D12_MESSAGE_ID lDenyIds[] = {
		D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE, // I'm really not sure how to avoid this message.
		D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,	  // This warning occurs when using capture frame while graphics
												  // debugging.
		D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE, // This warning occurs when using capture frame while graphics
												  // debugging.
	};

	D3D12_INFO_QUEUE_FILTER lNewFilter = {};
	// NewFilter.DenyList.NumCategories = _countof(Categories);
	// NewFilter.DenyList.pCategoryList = Categories;
	lNewFilter.DenyList.NumSeverities = _countof(lSeverities);
	lNewFilter.DenyList.pSeverityList = lSeverities;
	lNewFilter.DenyList.NumIDs		  = _countof(lDenyIds);
	lNewFilter.DenyList.pIDList		  = lDenyIds;

	RESULT_CONDITION_ENSURE(lInfoQueue->PushStorageFilter(&lNewFilter) == S_OK, Dx12FailedToCreateDevice, {});

#endif

	RESULT_OK();
	return lDevice;
}

INLINE ComPtr<ID3D12CommandQueue> PlatformManager::CreateCommandQueue(const ComPtr<ID3D12Device>	Device,
																	  const D3D12_COMMAND_LIST_TYPE Type,
																	  RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST({});
	RESULT_CONDITION_ENSURE(Device, NullPtr, {});

	ComPtr<ID3D12CommandQueue> lQueue;
	D3D12_COMMAND_QUEUE_DESC   lDesc{};
	lDesc.Type	   = Type;
	lDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	lDesc.Flags	   = D3D12_COMMAND_QUEUE_FLAG_NONE;
	lDesc.NodeMask = 0;

	RESULT_CONDITION_ENSURE(Device->CreateCommandQueue(&lDesc, IID_PPV_ARGS(&lQueue)) == S_OK,
							Dx12FailedToCreateCommandQueue, {});

	RESULT_OK();
	return lQueue;
}

INLINE bool PlatformManager::CheckTearingSupport(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST(false);

	BOOL lAllowTearing = FALSE;

	// Rather than create the DXGI 1.5 factory interface directly, we create the
	// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the
	// graphics debugging tools which will not support the 1.5 factory interface
	// until a future update.
	ComPtr<IDXGIFactory4> lFactory4;
	if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&lFactory4))))
	{
		ComPtr<IDXGIFactory5> lFactory5;
		if (SUCCEEDED(lFactory4.As(&lFactory5)))
		{
			if (FAILED(lFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &lAllowTearing,
													  sizeof lAllowTearing)))
			{
				lAllowTearing = FALSE;
			}
		}
	}

	RESULT_OK();
	return lAllowTearing == TRUE;
}

INLINE bool PlatformManager::CheckRaytracingSupport(const ComPtr<ID3D12Device> Device, RESULT_PARAM_IMPL)
{
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 lOptions5		   = {};
	BOOL							  lAllowRaytracing = FALSE;
	if (Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &lOptions5, sizeof lOptions5))
	{
		if (lOptions5.RaytracingTier > D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
		{
			lAllowRaytracing = TRUE;
		}
	}
	return lAllowRaytracing;
}

INLINE ComPtr<IDXGISwapChain4> PlatformManager::CreateSwapchain(const HWND						 Hwnd,
																const ComPtr<ID3D12CommandQueue> CommandQueue,
																const uint32_t Width, const uint32_t Height,
																const uint32_t BufferCount, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST({});
	RESULT_CONDITION_ENSURE(Hwnd && CommandQueue, NullPtr, {});
	RESULT_CONDITION_ENSURE(Width > 0 && Height > 0 && BufferCount > 0, ZeroSize, {});

	ComPtr<IDXGISwapChain4> lDxgiSwapChain4;
	ComPtr<IDXGIFactory4>	lDxgiFactory4;
	UINT					lCreateFactoryFlags = 0;
#ifdef DEBUG
	lCreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	RESULT_CONDITION_ENSURE(CreateDXGIFactory2(lCreateFactoryFlags, IID_PPV_ARGS(&lDxgiFactory4)) == S_OK,
							Dx12FailedToCreateFactory, {});

	DXGI_SWAP_CHAIN_DESC1 lSwapchainDesc = {};
	lSwapchainDesc.Width				 = Width;
	lSwapchainDesc.Height				 = Height;
	lSwapchainDesc.Format				 = DXGI_FORMAT_R8G8B8A8_UNORM;
	lSwapchainDesc.Stereo				 = FALSE;
	lSwapchainDesc.SampleDesc			 = {1, 0};
	lSwapchainDesc.BufferUsage			 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	lSwapchainDesc.BufferCount			 = BufferCount;
	lSwapchainDesc.Scaling				 = DXGI_SCALING_STRETCH;
	lSwapchainDesc.SwapEffect			 = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	lSwapchainDesc.AlphaMode			 = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	lSwapchainDesc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	ComPtr<IDXGISwapChain1> lSwapChain1;
	const HRESULT lResult = lDxgiFactory4->CreateSwapChainForHwnd(CommandQueue.Get(), Hwnd, &lSwapchainDesc, nullptr,
																  nullptr, &lSwapChain1);

	RESULT_CONDITION_ENSURE(lResult == S_OK, Dx12FailedToCreateSwapchain, {});

	// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
	// will be handled manually.
	RESULT_CONDITION_ENSURE(lDxgiFactory4->MakeWindowAssociation(Hwnd, DXGI_MWA_NO_ALT_ENTER) == S_OK,
							Dx12FailedToCreateSwapchain, {});

	RESULT_CONDITION_ENSURE(lSwapChain1.As(&lDxgiSwapChain4) == S_OK, Dx12FailedToCreateSwapchain, {});

	RESULT_OK();
	return lDxgiSwapChain4;
}

INLINE ComPtr<ID3D12DescriptorHeap> PlatformManager::CreateDescriptorHeap(const ComPtr<ID3D12Device>	   Device,
																		  const D3D12_DESCRIPTOR_HEAP_TYPE Type,
																		  const uint32_t NumDescriptors,
																		  RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST({});
	RESULT_CONDITION_ENSURE(Device, NullPtr, {});
	RESULT_CONDITION_ENSURE(NumDescriptors, ZeroSize, {});

	ComPtr<ID3D12DescriptorHeap> lDescriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC lDesc = {};
	lDesc.NumDescriptors			 = NumDescriptors;
	lDesc.Type						 = Type;

	RESULT_CONDITION_ENSURE(Device->CreateDescriptorHeap(&lDesc, IID_PPV_ARGS(&lDescriptorHeap)) == S_OK,
							Dx12FailedToCreateDescriptorHeap, {});

	RESULT_OK();
	return lDescriptorHeap;
}

INLINE void PlatformManager::UpdateRenderTargetViews(const ComPtr<ID3D12Device>			Device,
													 const ComPtr<IDXGISwapChain4>		Swapchain,
													 const ComPtr<ID3D12DescriptorHeap> DescriptorHeap,
													 RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(Device && Swapchain && DescriptorHeap, NullPtr);

	const auto lRtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE lRtvHandle(DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (int lI = 0; lI < NUM_FRAMES; ++lI)
	{
		ComPtr<ID3D12Resource> lBackBuffer;

		RESULT_CONDITION_ENSURE(Swapchain->GetBuffer(lI, IID_PPV_ARGS(&lBackBuffer)) == S_OK,
								Dx12FailedToGetBackBuffer);

		Device->CreateRenderTargetView(lBackBuffer.Get(), nullptr, lRtvHandle);

		mBackBuffers[lI] = lBackBuffer;

		lRtvHandle.Offset(lRtvDescriptorSize);
	}
	RESULT_OK();
}

INLINE ComPtr<ID3D12CommandAllocator> PlatformManager::CreateCommandAllocator(const ComPtr<ID3D12Device>	Device,
																			  const D3D12_COMMAND_LIST_TYPE Type,
																			  RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST({});
	RESULT_CONDITION_ENSURE(Device, NullPtr, {});

	ComPtr<ID3D12CommandAllocator> lCommandAllocator;

	RESULT_CONDITION_ENSURE(Device->CreateCommandAllocator(Type, IID_PPV_ARGS(&lCommandAllocator)) == S_OK,
							Dx12FailedToCreateCommandAllocator, {})

	RESULT_OK();
	return lCommandAllocator;
}

INLINE ComPtr<ID3D12GraphicsCommandList> PlatformManager::CreateCommandList(
	const ComPtr<ID3D12Device> Device, const ComPtr<ID3D12CommandAllocator> CommandAllocator,
	const D3D12_COMMAND_LIST_TYPE Type, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST({});
	RESULT_CONDITION_ENSURE(Device && CommandAllocator, NullPtr, {});

	ComPtr<ID3D12GraphicsCommandList> lCommandList;
	RESULT_CONDITION_ENSURE(
		Device->CreateCommandList(0, Type, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&lCommandList)) == S_OK,
		Dx12FailedToCreateCommandList, {});

	RESULT_CONDITION_ENSURE(lCommandList->Close() == S_OK, Dx12FailedToCloseCommandList, {});

	RESULT_OK();
	return lCommandList;
}

INLINE ComPtr<ID3D12Fence> PlatformManager::CreateFence(const ComPtr<ID3D12Device> Device, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST({});
	RESULT_CONDITION_ENSURE(Device, NullPtr, {});

	ComPtr<ID3D12Fence> lFence;

	RESULT_CONDITION_ENSURE(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&lFence)) == S_OK,
							Dx12FailedToCreateFence, {});

	RESULT_OK();
	return lFence;
}

INLINE HANDLE PlatformManager::CreateFenceEvent(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST(nullptr);
	const HANDLE lFenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
	RESULT_CONDITION_ENSURE(lFenceEvent, Dx12FailedToCreateFenceEvent, nullptr);
	return lFenceEvent;
}

INLINE uint64_t PlatformManager::SignalFence(const ComPtr<ID3D12CommandQueue> CommandQueue,
											 const ComPtr<ID3D12Fence> Fence, uint64_t& FenceValue, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST(UINT64_MAX);
	RESULT_CONDITION_ENSURE(CommandQueue && Fence, NullPtr, UINT64_MAX);

	const uint64_t lFenceValueForSignal = ++FenceValue;

	RESULT_CONDITION_ENSURE(CommandQueue->Signal(Fence.Get(), lFenceValueForSignal) == S_OK, Dx12FailedToSignalFence,
							UINT64_MAX);

	RESULT_OK();
	return lFenceValueForSignal;
}

INLINE void PlatformManager::WaitForFenceValue(const ComPtr<ID3D12Fence> Fence, const uint64_t FenceValue,
											   const HANDLE FenceEvent, const uint64_t MsDuration, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(Fence && FenceEvent, NullPtr);
	RESULT_CONDITION_ENSURE(MsDuration, ZeroSize);

	if (Fence->GetCompletedValue() < FenceValue)
	{
		RESULT_CONDITION_ENSURE(Fence->SetEventOnCompletion(FenceValue, FenceEvent) == S_OK, Dx12FailedToWaitForFence);
		::WaitForSingleObject(FenceEvent, static_cast<DWORD>(MsDuration));
	}

	RESULT_OK();
}

INLINE void PlatformManager::Flush(const ComPtr<ID3D12CommandQueue> CommandQueue, const ComPtr<ID3D12Fence> Fence,
								   uint64_t& FenceValue, const HANDLE FenceEvent, RESULT_PARAM_IMPL)
{
	ManagerWait<Render::Manager>{};
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(CommandQueue && Fence && FenceEvent, NullPtr);
	RESULT_ENSURE_CALL(const uint64_t lFenceValueForSignal = SignalFence(CommandQueue, Fence, FenceValue));
	RESULT_ENSURE_CALL(WaitForFenceValue(Fence, lFenceValueForSignal, FenceEvent));
	RESULT_OK();
}

INLINE void PlatformManager::Flush(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(Flush(mCommandQueue, mFence, mFenceValue, mFenceEvent, RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE void PlatformManager::WaitFrameFences(RESULT_PARAM_IMPL)
{
	for (uint32_t i = 0; i < NUM_FRAMES; ++i)
	{
		RESULT_ENSURE_CALL(WaitForFenceValue(mFence, mFrameFenceValues[i], mFenceEvent, UINT64_MAX, RESULT_ARG_PASS));
	}
}

} // namespace Render

CLASS_VALIDATION(Render::PlatformManager);

#endif
