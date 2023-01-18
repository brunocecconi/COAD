
#if VULKAN_ENABLED

#undef VK_ENABLE_VALIDATION_LAYER
#ifndef NDEBUG
#define VK_ENABLE_VALIDATION_LAYER 1
#else
#define VK_ENABLE_VALIDATION_LAYER 0
#endif

LOG_DEFINE(RenderApiVulkan);

#include <EASTL/optional.h>

/** \file VulkanManager.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

namespace Render::Api
{

/**
 * \brief Render api allocator.
 *
 */
class Allocator: public eastl::allocator
{
public:
	EXPLICIT Allocator(const char* Name = DEBUG_NAME("Render::Api::Vulkan"));

public:
	void* allocate(size_t N, int32_t /*flags*/ = 0);
	void* allocate(size_t N, size_t Alignment, size_t AlignmentOffset, int32_t /*flags*/ = 0);
	void* reallocate(void* Original, size_t N, int32_t /*flags*/ = 0);
	void* reallocate(void* Original, size_t N, size_t Alignment, size_t AlignmentOffset, int32_t /*flags*/ = 0);
	void  deallocate(void* P, size_t N);
};
using default_allocator_t = Allocator;

INLINE Allocator::Allocator(const char* Name) : eastl::allocator{Name}
{
}

INLINE void* Allocator::allocate(const size_t N, int32_t)
{
	return mi_malloc(N);
}

INLINE void* Allocator::allocate(const size_t N, const size_t Alignment, size_t, int32_t)
{
	return mi_malloc_aligned(N, Alignment);
}

INLINE void* Allocator::reallocate(void* Original, const size_t N, int32_t)
{
	return mi_realloc(Original, N);
}

INLINE void* Allocator::reallocate(void* Original, const size_t N, const size_t Alignment, size_t, int32_t)
{
	return mi_realloc_aligned(Original, N, Alignment);
}

INLINE void Allocator::deallocate(void* P, size_t)
{
	mi_free(P);
}

using shader_compile_data_t = RawBuffer<char, default_allocator_t>;

#ifndef RELEASE
enum class EShaderKind
{
	eVertex	  = shaderc_vertex_shader,
	eFragment = shaderc_fragment_shader,
	eCompute  = shaderc_compute_shader,
	eGeometry = shaderc_geometry_shader
};

class Manager;

/**
 * \brief Shader manager.
 *
 */
class ShaderManager
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(ShaderManager);

private:
	ShaderManager();

public:
	~ShaderManager();

public:
	static constexpr eastl::string_view MACRO_NAME[] = {
		const_cast<char*>("VERTEX_SHADER"), const_cast<char*>("FRAGMENT_SHADER"), const_cast<char*>("COMPUTE_SHADER"),
		const_cast<char*>("GEOMETRY_SHADER")};

	template<EShaderKind Kind>
	shader_compile_data_t Compile(eastl::string_view SourceText, eastl::string_view Name, RESULT_PARAM_DEFINE) const;

private:
	friend class Manager;
	shaderc_compiler_t		  mShaderc{};
	shaderc_compile_options_t mShadercOptions{};
};
#endif

/**
 * \brief Render api mesh class.
 *
 * Stores a vulkan buffer based on outside vertices.
 *
 * Obs: copy semantics are do by just copying the pointers, not the data.
 * This procedure is default because the buffer creation is based on gpu calls.
 *
 */
class Mesh
{
	CLASS_BODY_NON_COPYABLE_OMIT_MOVE(Mesh);

public:
	Mesh() = default;
	Mesh(Manager* Manager, VkPhysicalDevice PhysicalDevice, VkDevice Device,
		 eastl::vector<Vertex, default_allocator_t>& Vertices, RESULT_PARAM_DEFINE);
	~Mesh();

public:
	Mesh(Mesh&& Other) NOEXCEPT;
	Mesh& operator=(Mesh&& Other) NOEXCEPT;

public:
	NODISCARD static Mesh Square(Manager* Manager, VkPhysicalDevice PhysicalDevice, VkDevice Device, RESULT_PARAM_DEFINE);

public:
	void			   DestroyVertexBuffer(RESULT_PARAM_DEFINE);
	NODISCARD uint32_t GetVertexSize() const;
	NODISCARD VkBuffer GetVertexBuffer() const;

private:
	void	 CreateVertexBuffer(eastl::vector<Vertex, default_allocator_t>& Vertices, RESULT_PARAM_DEFINE);
	uint32_t FindMemoryTypeIndex(uint32_t AllowedTypes, VkMemoryPropertyFlags PropertyFlags, RESULT_PARAM_DEFINE) const;

private:
	Manager*		 mManager{};
	uint32_t		 mVertexSize{};
	VkBuffer		 mVertexBuffer{};
	VkDeviceMemory	 mVertexBufferMemory{};
	VkPhysicalDevice mPhysicalDevice{};
	VkDevice		 mDevice{};
};

/**
 * @brief Render api manager class.
 *
 */
class Manager
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(Manager);

private:
	Manager();

public:
	~Manager();

private:
	void Initialize(RESULT_PARAM_DEFINE);
	void Update(RESULT_PARAM_DEFINE);
	void Finalize(RESULT_PARAM_DEFINE);
	void ResizeFramebuffer(glm::uvec2 NewSize, RESULT_PARAM_DEFINE);
	void MarkDirtyFramebufferSize(RESULT_PARAM_DEFINE);

private:
	void ToggleFullscreen(RESULT_PARAM_DEFINE);

private:
	struct QueueFamilyIndices
	{
		eastl::optional<uint32_t> GraphicsFamily;
		eastl::optional<uint32_t> PresentFamily;

		NODISCARD bool IsComplete() const;
	};

	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR						   Capabilities{};
		RawBuffer<VkSurfaceFormatKHR, default_allocator_t> Formats{default_allocator_t{}};
		RawBuffer<VkPresentModeKHR, default_allocator_t>   PresentModes{default_allocator_t{}};
	};

	void		   CreateInstance(RESULT_PARAM_DEFINE);
	void		   CreateSurface(RESULT_PARAM_DEFINE);
	void		   PickPhysicalDevice(RESULT_PARAM_DEFINE);
	void		   CreateLogicalDevice(RESULT_PARAM_DEFINE);
	void		   CreateSwapchain(RESULT_PARAM_DEFINE);
	void		   CreateImageViews(RESULT_PARAM_DEFINE);
	void		   CreateRenderPass(RESULT_PARAM_DEFINE);
	void		   CreateGraphicsPipeline(RESULT_PARAM_DEFINE);
	void		   CreateFramebuffers(RESULT_PARAM_DEFINE);
	VkShaderModule CreateShaderModule(const shader_compile_data_t& Data, RESULT_PARAM_DEFINE) const;

	void CreateCommandPool(RESULT_PARAM_DEFINE);
	void CreateCommandBuffers(RESULT_PARAM_DEFINE);
	void RecordCommandBuffer(VkCommandBuffer CommandBuffer, uint32_t ImageIndex, RESULT_PARAM_DEFINE);
	void CreateSyncObjects(RESULT_PARAM_DEFINE);

	void DestroySwapchain(RESULT_PARAM_DEFINE);
	void DestroySyncObjects(RESULT_PARAM_DEFINE);
	void RecreateSwapchain(RESULT_PARAM_DEFINE);
	void RecreateSyncObjects(RESULT_PARAM_DEFINE);

private:
	NODISCARD bool						IsDeviceSuitable(VkPhysicalDevice Device) const;
	NODISCARD QueueFamilyIndices		FindQueueFamilies(VkPhysicalDevice Device) const;
	NODISCARD bool						CheckDeviceExtensionSupport(VkPhysicalDevice Device) const;
	NODISCARD SwapchainSupportDetails	QuerySwapchainSupport(VkPhysicalDevice Device) const;
	NODISCARD static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
		const RawBuffer<VkSurfaceFormatKHR, default_allocator_t>& AvailableFormats);
	NODISCARD static VkPresentModeKHR ChooseSwapPresentMode(
		const RawBuffer<VkPresentModeKHR, default_allocator_t>& AvailablePresentModes);
	NODISCARD VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilities) const;

private:
	NODISCARD static void* Allocation(void* UserData, size_t Size, size_t Alignment,
									  VkSystemAllocationScope AllocationScope);
	NODISCARD static void* Reallocation(void* UserData, void* Original, size_t Size, size_t Alignment,
										VkSystemAllocationScope AllocationScope);
	static void			   Free(void* UserData, void* Memory);
	static void InternalAllocationNotification(void* UserData, size_t Size, VkInternalAllocationType AllocationType,
											   VkSystemAllocationScope AllocationScope);
	static void InternalFreeNotification(void* UserData, size_t Size, VkInternalAllocationType AllocationType,
										 VkSystemAllocationScope AllocationScope);

private:
#if VK_ENABLE_VALIDATION_LAYER
	NODISCARD bool			  CheckValidationLayoutSupport() const;
	static void				  PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& CreateInfo);
	void					  SetupDebugMessenger(RESULT_PARAM_DEFINE);
	NODISCARD static VkResult CreateDebugUtilsMessengerExt(VkInstance								 Instance,
														   const VkDebugUtilsMessengerCreateInfoEXT* CreateInfo,
														   const VkAllocationCallbacks*				 Allocator,
														   VkDebugUtilsMessengerEXT*				 DebugMessenger);
	static void DestroyDebugUtilsMessengerExt(VkInstance Instance, VkDebugUtilsMessengerEXT DebugMessenger,
											  const VkAllocationCallbacks* Allocator);
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT		MessageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT				MessageType,
														const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,
														void*										UserData);
#endif

private:
	friend class Render::Manager;
	friend class Mesh;

private:
	Engine::Window& mWindow;
	bool			mVsync = true;
	ShaderManager	mShaderManager{};

private:
	uint32_t									  mMaxFrames = 3u, mCurrentFrame{};
	VkInstance									  mInstance{};
	VkPhysicalDevice							  mPhysicalDevice{};
	VkSurfaceKHR								  mSurface{};
	VkDevice									  mDevice{};
	VkQueue										  mGraphicsQueue{}, mPresentQueue{};
	VkSwapchainKHR								  mSwapchain{};
	RawBuffer<VkImage, default_allocator_t>		  mSwapchainImages{mAllocator};
	VkFormat									  mSwapchainImageFormat;
	VkExtent2D									  mSwapchainExtent;
	RawBuffer<VkImageView, default_allocator_t>	  mSwapchainImageViews{mAllocator};
	RawBuffer<VkFramebuffer, default_allocator_t> mSwapchainFramebuffers{mAllocator};
	VkRenderPass								  mRenderPass{};
	VkPipelineLayout							  mPipelineLayout{};
	VkPipeline									  mGraphicsPipeline{};

	VkCommandPool									mCommandPool{};
	RawBuffer<VkCommandBuffer, default_allocator_t> mCommandBuffers{mAllocator};
	RawBuffer<VkSemaphore, default_allocator_t>		mImageAvailableSemaphores{mAllocator};
	RawBuffer<VkSemaphore, default_allocator_t>		mRenderFinishedSemaphores{mAllocator};
	RawBuffer<VkFence, default_allocator_t>			mInFlightFences{mAllocator};

private:
	bool mMarkDirtyFramebufferSize{};

private:
	default_allocator_t	  mAllocator{};
	VkAllocationCallbacks mAllocatorCallbacks{};

private:
	Mesh mMesh;

private:
#if EDITOR
	friend class Editor::Manager;
	bool mEditor = false;
#endif
#if VK_ENABLE_VALIDATION_LAYER
	eastl::vector<const char*, default_allocator_t> mValidationLayers{{"VK_LAYER_KHRONOS_validation"}, mAllocator};
	VkDebugUtilsMessengerEXT						mDebugMessenger{};
#endif
	eastl::vector<const char*, default_allocator_t> mDeviceExtensions{{VK_KHR_SWAPCHAIN_EXTENSION_NAME}, mAllocator};
};

#ifndef RELEASE

INLINE ShaderManager::ShaderManager()
{
	mShadercOptions = shaderc_compile_options_initialize();
	shaderc_compile_options_set_source_language(mShadercOptions, shaderc_source_language_glsl); // Temporary

	shaderc_compile_options_set_optimization_level(mShadercOptions,
#ifdef RELEASE
												   shaderc_optimization_level_performance
#else
												   shaderc_optimization_level_zero
#endif
	);

	mShaderc = shaderc_compiler_initialize();
}

INLINE ShaderManager::~ShaderManager()
{
	shaderc_compiler_release(mShaderc);
	shaderc_compile_options_release(mShadercOptions);
}

template<EShaderKind Kind>
shader_compile_data_t ShaderManager::Compile(const eastl::string_view SourceText, const eastl::string_view Name,
											 RESULT_PARAM_IMPL) const
{
	shader_compile_data_t lBuffer{default_allocator_t{DEBUG_NAME_VAL("Render::Api::Vulkan::ShaderManager")}};
	RESULT_ENSURE_LAST(lBuffer);

	LOGC(Verbose, RenderApiVulkan, "Compiling shader '%s'...", Name.data());

	shaderc_compile_options_add_macro_definition(mShadercOptions, MACRO_NAME[0].data(), MACRO_NAME[0].size(), "0", 1);
	shaderc_compile_options_add_macro_definition(mShadercOptions, MACRO_NAME[1].data(), MACRO_NAME[1].size(), "0", 1);
	shaderc_compile_options_add_macro_definition(mShadercOptions, MACRO_NAME[2].data(), MACRO_NAME[2].size(), "0", 1);
	shaderc_compile_options_add_macro_definition(mShadercOptions, MACRO_NAME[3].data(), MACRO_NAME[3].size(), "0", 1);

	constexpr auto lName = MACRO_NAME[static_cast<uint32_t>(Kind)];
	shaderc_compile_options_add_macro_definition(mShadercOptions, lName.data(), lName.size(), "1", 1);
	const auto lResult =
		shaderc_compile_into_spv(mShaderc, SourceText.data(), SourceText.size(), static_cast<shaderc_shader_kind>(Kind),
								 Name.data(), "main", mShadercOptions);

	if (const auto lResultStatus = shaderc_result_get_compilation_status(lResult);
		lResultStatus != shaderc_compilation_status_success)
	{
		LOGC(Error, RenderApiVulkan, "Failed to compile shader: %s", shaderc_result_get_error_message(lResult));
		RESULT_ERROR(RenderPlatformFailedToCompileShader, lBuffer);
	}

	lBuffer.Resize(shaderc_result_get_length(lResult));
	memcpy(lBuffer.Data(), shaderc_result_get_bytes(lResult), lBuffer.Size());

	RESULT_OK();
	LOGC(Verbose, RenderApiVulkan, "Success shader compilation for '%s'.", Name.data());
	return lBuffer;
}
#endif

INLINE Manager::Manager() : mWindow{Engine::Manager::Instance().GetWindow()}
{
}

INLINE Manager::~Manager()
{
}

INLINE void Manager::Initialize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	RESULT_ENSURE_CALL(CreateInstance(RESULT_ARG_PASS));
#if VK_ENABLE_VALIDATION_LAYER
	RESULT_ENSURE_CALL(SetupDebugMessenger(RESULT_ARG_PASS));
#endif
	RESULT_ENSURE_CALL(CreateSurface(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(PickPhysicalDevice(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateLogicalDevice(RESULT_ARG_PASS));

	RESULT_ENSURE_CALL(mMesh = Mesh::Square(this, mPhysicalDevice, mDevice, RESULT_ARG_PASS));

	RESULT_ENSURE_CALL(CreateSwapchain(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateImageViews(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateRenderPass(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateGraphicsPipeline(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateFramebuffers(RESULT_ARG_PASS));

	RESULT_ENSURE_CALL(CreateCommandPool(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateCommandBuffers(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateSyncObjects(RESULT_ARG_PASS));

	RESULT_OK();
}

INLINE void Manager::Update(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	// ImGui_ImplVulkan_NewFrame();
	// ImGui::NewFrame();

	vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

	uint32_t lImageIndex{};
	VkResult lResult = vkAcquireNextImageKHR(mDevice, mSwapchain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame],
											 VK_NULL_HANDLE, &lImageIndex);

	if (lResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RESULT_ENSURE_CALL(RecreateSwapchain(RESULT_ARG_PASS));
		RESULT_OK();
		return;
	}

	RESULT_CONDITION_ENSURE(lResult == VK_SUCCESS || lResult == VK_SUBOPTIMAL_KHR, VulkanFailedToAcquireSwapchainImage);

	vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

	vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], 0);
	RESULT_ENSURE_CALL(RecordCommandBuffer(mCommandBuffers[mCurrentFrame], lImageIndex, RESULT_ARG_PASS));

	VkSubmitInfo lSubmitInfo{};
	lSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	const VkSemaphore			   lWaitSemaphores[] = {mImageAvailableSemaphores[mCurrentFrame]};
	constexpr VkPipelineStageFlags lWaitStages[]	 = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	lSubmitInfo.waitSemaphoreCount					 = 1;
	lSubmitInfo.pWaitSemaphores						 = lWaitSemaphores;
	lSubmitInfo.pWaitDstStageMask					 = lWaitStages;

	lSubmitInfo.commandBufferCount = 1;
	lSubmitInfo.pCommandBuffers	   = &mCommandBuffers[mCurrentFrame];

	const VkSemaphore lSignalSemaphores[] = {mRenderFinishedSemaphores[mCurrentFrame]};
	lSubmitInfo.signalSemaphoreCount	  = 1;
	lSubmitInfo.pSignalSemaphores		  = lSignalSemaphores;

	RESULT_CONDITION_ENSURE(vkQueueSubmit(mGraphicsQueue, 1, &lSubmitInfo, mInFlightFences[mCurrentFrame]) ==
								VK_SUCCESS,
							VulkanFailedToSubmitGraphicsQueue);

	VkPresentInfoKHR lPresentInfo{};
	lPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	lPresentInfo.waitSemaphoreCount = 1;
	lPresentInfo.pWaitSemaphores	= lSignalSemaphores;

	const VkSwapchainKHR lSwapChains[] = {mSwapchain};
	lPresentInfo.swapchainCount		   = 1;
	lPresentInfo.pSwapchains		   = lSwapChains;

	lPresentInfo.pImageIndices = &lImageIndex;

	lResult = vkQueuePresentKHR(mPresentQueue, &lPresentInfo);

	if (lResult == VK_ERROR_OUT_OF_DATE_KHR || lResult == VK_SUBOPTIMAL_KHR || mMarkDirtyFramebufferSize)
	{
		mMarkDirtyFramebufferSize = false;
		RESULT_ENSURE_CALL(RecreateSwapchain(RESULT_ARG_PASS));
	}

	mCurrentFrame = (mCurrentFrame + 1u) % mMaxFrames;

	// ImGui::Render();

	RESULT_OK();
}

INLINE void Manager::Finalize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(mInstance, NullPtr);
	RESULT_CONDITION_ENSURE(mSurface, NullPtr);
	RESULT_CONDITION_ENSURE(mSwapchain, NullPtr);

	vkDeviceWaitIdle(mDevice);

	RESULT_ENSURE_CALL(mMesh.DestroyVertexBuffer(RESULT_ARG_PASS));

	RESULT_ENSURE_CALL(DestroySwapchain(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(DestroySyncObjects(RESULT_ARG_PASS));

	vkDestroyCommandPool(mDevice, mCommandPool, &mAllocatorCallbacks);

	vkDestroyPipeline(mDevice, mGraphicsPipeline, &mAllocatorCallbacks);
	vkDestroyPipelineLayout(mDevice, mPipelineLayout, &mAllocatorCallbacks);
	vkDestroyRenderPass(mDevice, mRenderPass, &mAllocatorCallbacks);

	vkDestroyDevice(mDevice, &mAllocatorCallbacks);

#if VK_ENABLE_VALIDATION_LAYER
	DestroyDebugUtilsMessengerExt(mInstance, mDebugMessenger, &mAllocatorCallbacks);
#endif

	vkDestroySurfaceKHR(mInstance, mSurface, &mAllocatorCallbacks);
	vkDestroyInstance(mInstance, &mAllocatorCallbacks);

	RESULT_OK();
}

INLINE void Manager::ResizeFramebuffer(const glm::uvec2 NewSize, RESULT_PARAM_IMPL)
{
	RESULT_CONDITION_ENSURE(Render::Manager::Instance().IsInThread(), CurrentThreadIsNotTheRequiredOne);

	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(NewSize.x > 0 && NewSize.y > 0, ZeroSize);
	RESULT_ENSURE_CALL(RecreateSwapchain(RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE void Manager::MarkDirtyFramebufferSize(RESULT_PARAM_IMPL)
{
	RESULT_CONDITION_ENSURE(Render::Manager::Instance().IsInThread(), CurrentThreadIsNotTheRequiredOne);
	RESULT_ENSURE_LAST();
	LOGC(Info, RenderApiVulkan, "Marking dirty framebuffer size. New size: [%u,%u].", mSwapchainExtent.width,
		 mSwapchainExtent.height);
	mMarkDirtyFramebufferSize = true;
	RESULT_OK();
}

INLINE void Manager::ToggleFullscreen(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(ResizeFramebuffer(mWindow.GetSize(), RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE bool Manager::QueueFamilyIndices::IsComplete() const
{
	return GraphicsFamily.has_value() && PresentFamily.has_value();
}

INLINE void Manager::CreateInstance(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

#if VK_ENABLE_VALIDATION_LAYER
	ENFORCE_MSG(CheckValidationLayoutSupport(), "Vulkan validation layers requested, but not available.");
#endif

	// Setup allocator
	mAllocatorCallbacks.pUserData			  = &mAllocator;
	mAllocatorCallbacks.pfnAllocation		  = Allocation;
	mAllocatorCallbacks.pfnFree				  = Free;
	mAllocatorCallbacks.pfnInternalAllocation = InternalAllocationNotification;
	mAllocatorCallbacks.pfnInternalFree		  = InternalFreeNotification;
	mAllocatorCallbacks.pfnReallocation		  = Reallocation;

	VkApplicationInfo lAppInfo{};
	lAppInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	lAppInfo.pApplicationName	= "COAD";
	lAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	lAppInfo.pEngineName		= "No Engine";
	lAppInfo.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
	lAppInfo.apiVersion			= VK_API_VERSION_1_0;

	VkInstanceCreateInfo lInstanceCi{};
	lInstanceCi.sType			 = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	lInstanceCi.pApplicationInfo = &lAppInfo;

	constexpr char* lExtensions[] = {
		const_cast<char*>(VK_KHR_SURFACE_EXTENSION_NAME),
		const_cast<char*>(VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
#if VK_ENABLE_VALIDATION_LAYER
			,
		const_cast<char*>(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
#endif
	};

	lInstanceCi.enabledExtensionCount	= sizeof lExtensions / sizeof(char*);
	lInstanceCi.ppEnabledExtensionNames = lExtensions;

#if VK_ENABLE_VALIDATION_LAYER
	VkDebugUtilsMessengerCreateInfoEXT lDebugCreateInfo{};
	lInstanceCi.enabledLayerCount	= static_cast<uint32_t>(mValidationLayers.size());
	lInstanceCi.ppEnabledLayerNames = mValidationLayers.data();

	PopulateDebugMessengerCreateInfo(lDebugCreateInfo);
	lInstanceCi.pNext = &lDebugCreateInfo;
#else
	lInstanceCi.enabledLayerCount = 0;
	lInstanceCi.pNext			  = nullptr;
#endif

	VkResult lResult;
	RESULT_CONDITION_ENSURE((lResult = vkCreateInstance(&lInstanceCi, &mAllocatorCallbacks, &mInstance)) == VK_SUCCESS,
							VulkanFailedToCreateInstance);

	RESULT_OK();
}

INLINE void Manager::CreateSurface(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	VkWin32SurfaceCreateInfoKHR lCreateInfo{};
	lCreateInfo.sType	  = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	lCreateInfo.hwnd	  = mWindow.GetHandle();
	lCreateInfo.hinstance = GetModuleHandle(nullptr);
	RESULT_CONDITION_ENSURE(vkCreateWin32SurfaceKHR(mInstance, &lCreateInfo, &mAllocatorCallbacks, &mSurface) ==
								VK_SUCCESS,
							VulkanFailedToCreateSurface);
	RESULT_OK();
}

INLINE void Manager::PickPhysicalDevice(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	uint32_t lDeviceCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &lDeviceCount, nullptr);

	RESULT_CONDITION_ENSURE(lDeviceCount > 0, VulkanFailedToFindGpuWithVulkanSupport);

	RawBuffer<VkPhysicalDevice, default_allocator_t> lDevices{lDeviceCount, mAllocator};
	vkEnumeratePhysicalDevices(mInstance, &lDeviceCount, lDevices.Data());

	for (const auto& lDevice: lDevices)
	{
		if (IsDeviceSuitable(lDevice))
		{
			mPhysicalDevice = lDevice;
			break;
		}
	}

	RESULT_CONDITION_ENSURE(mPhysicalDevice, VulkanFailedToPickPhysicalDevice);
	RESULT_OK();
}

INLINE void Manager::CreateLogicalDevice(RESULT_PARAM_IMPL)
{
	QueueFamilyIndices lIndices = FindQueueFamilies(mPhysicalDevice);

	eastl::vector<VkDeviceQueueCreateInfo, default_allocator_t> lQueueCreateInfos{mAllocator};
	eastl::set<uint32_t> lUniqueQueueFamilies = {lIndices.GraphicsFamily.value(), lIndices.PresentFamily.value()};
	lQueueCreateInfos.reserve(lUniqueQueueFamilies.size());

	constexpr float lQueuePriority = 1.0f;
	for (uint32_t lQueueFamily: lUniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo lQueueCreateInfo{};
		lQueueCreateInfo.sType			  = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		lQueueCreateInfo.queueFamilyIndex = lQueueFamily;
		lQueueCreateInfo.queueCount		  = 1;
		lQueueCreateInfo.pQueuePriorities = &lQueuePriority;
		lQueueCreateInfos.push_back(lQueueCreateInfo);
	}

	VkPhysicalDeviceFeatures lDeviceFeatures{};

	VkDeviceCreateInfo lCreateInfo{};
	lCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	lCreateInfo.pQueueCreateInfos	 = lQueueCreateInfos.data();
	lCreateInfo.queueCreateInfoCount = 1;

	lCreateInfo.pEnabledFeatures = &lDeviceFeatures;

	lCreateInfo.enabledExtensionCount	= static_cast<uint32_t>(mDeviceExtensions.size());
	lCreateInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

#if VK_ENABLE_VALIDATION_LAYER
	lCreateInfo.enabledLayerCount	= static_cast<uint32_t>(mValidationLayers.size());
	lCreateInfo.ppEnabledLayerNames = mValidationLayers.data();
#else
	lCreateInfo.enabledLayerCount = 0;
#endif

	RESULT_CONDITION_ENSURE(vkCreateDevice(mPhysicalDevice, &lCreateInfo, &mAllocatorCallbacks, &mDevice) == VK_SUCCESS,
							VulkanFailedToCreateDevice);

	vkGetDeviceQueue(mDevice, lIndices.GraphicsFamily.value(), 0, &mGraphicsQueue);
	vkGetDeviceQueue(mDevice, lIndices.PresentFamily.value(), 0, &mPresentQueue);
}

INLINE void Manager::CreateSwapchain(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	const SwapchainSupportDetails lSwapChainSupport = QuerySwapchainSupport(mPhysicalDevice);

	const VkSurfaceFormatKHR lSurfaceFormat = ChooseSwapSurfaceFormat(lSwapChainSupport.Formats);
	const VkPresentModeKHR	 lPresentMode	= ChooseSwapPresentMode(lSwapChainSupport.PresentModes);
	const VkExtent2D		 lExtent		= ChooseSwapExtent(lSwapChainSupport.Capabilities);

	uint32_t lImageCount = lSwapChainSupport.Capabilities.minImageCount + 1;
	if (lSwapChainSupport.Capabilities.maxImageCount > 0 && lImageCount > lSwapChainSupport.Capabilities.maxImageCount)
	{
		lImageCount = lSwapChainSupport.Capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR lCreateInfo{};
	lCreateInfo.sType	= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	lCreateInfo.surface = mSurface;

	lCreateInfo.minImageCount	 = lImageCount;
	lCreateInfo.imageFormat		 = lSurfaceFormat.format;
	lCreateInfo.imageColorSpace	 = lSurfaceFormat.colorSpace;
	lCreateInfo.imageExtent		 = lExtent;
	lCreateInfo.imageArrayLayers = 1;
	lCreateInfo.imageUsage		 = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices lIndices				 = FindQueueFamilies(mPhysicalDevice);
	const uint32_t	   lQueueFamilyIndices[] = {lIndices.GraphicsFamily.value(), lIndices.PresentFamily.value()};

	if (lIndices.GraphicsFamily != lIndices.PresentFamily)
	{
		lCreateInfo.imageSharingMode	  = VK_SHARING_MODE_CONCURRENT;
		lCreateInfo.queueFamilyIndexCount = 2;
		lCreateInfo.pQueueFamilyIndices	  = lQueueFamilyIndices;
	}
	else
	{
		lCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	lCreateInfo.preTransform   = lSwapChainSupport.Capabilities.currentTransform;
	lCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	lCreateInfo.presentMode	   = lPresentMode;
	lCreateInfo.clipped		   = VK_TRUE;

	lCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	RESULT_CONDITION_ENSURE(vkCreateSwapchainKHR(mDevice, &lCreateInfo, &mAllocatorCallbacks, &mSwapchain) ==
								VK_SUCCESS,
							VulkanFailedToCreateSwapchain);

	vkGetSwapchainImagesKHR(mDevice, mSwapchain, &lImageCount, nullptr);
	mSwapchainImages.Resize(lImageCount);
	vkGetSwapchainImagesKHR(mDevice, mSwapchain, &lImageCount, mSwapchainImages.Data());

	mSwapchainImageFormat = lSurfaceFormat.format;
	mSwapchainExtent	  = lExtent;
	RESULT_OK();
}

INLINE void Manager::CreateImageViews(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	mSwapchainImageViews.Resize(mSwapchainImages.Size());

	for (size_t lI = 0; lI < mSwapchainImages.Size(); lI++)
	{
		VkImageViewCreateInfo lCreateInfo{};
		lCreateInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		lCreateInfo.image							= mSwapchainImages[lI];
		lCreateInfo.viewType						= VK_IMAGE_VIEW_TYPE_2D;
		lCreateInfo.format							= mSwapchainImageFormat;
		lCreateInfo.components.r					= VK_COMPONENT_SWIZZLE_IDENTITY;
		lCreateInfo.components.g					= VK_COMPONENT_SWIZZLE_IDENTITY;
		lCreateInfo.components.b					= VK_COMPONENT_SWIZZLE_IDENTITY;
		lCreateInfo.components.a					= VK_COMPONENT_SWIZZLE_IDENTITY;
		lCreateInfo.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		lCreateInfo.subresourceRange.baseMipLevel	= 0;
		lCreateInfo.subresourceRange.levelCount		= 1;
		lCreateInfo.subresourceRange.baseArrayLayer = 0;
		lCreateInfo.subresourceRange.layerCount		= 1;

		RESULT_CONDITION_ENSURE(
			vkCreateImageView(mDevice, &lCreateInfo, &mAllocatorCallbacks, &mSwapchainImageViews[lI]) == VK_SUCCESS,
			VulkanFailedToCreateSwapchainImageView);
	}
	RESULT_OK();
}

INLINE void Manager::CreateRenderPass(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	VkAttachmentDescription lColorAttachment{};
	lColorAttachment.format			= mSwapchainImageFormat;
	lColorAttachment.samples		= VK_SAMPLE_COUNT_1_BIT;
	lColorAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	lColorAttachment.storeOp		= VK_ATTACHMENT_STORE_OP_STORE;
	lColorAttachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	lColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	lColorAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	lColorAttachment.finalLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference lColorAttachmentRef{};
	lColorAttachmentRef.attachment = 0;
	lColorAttachmentRef.layout	   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription lSubpass{};
	lSubpass.pipelineBindPoint	  = VK_PIPELINE_BIND_POINT_GRAPHICS;
	lSubpass.colorAttachmentCount = 1;
	lSubpass.pColorAttachments	  = &lColorAttachmentRef;

	VkSubpassDependency lDependency{};
	lDependency.srcSubpass	  = VK_SUBPASS_EXTERNAL;
	lDependency.dstSubpass	  = 0;
	lDependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	lDependency.srcAccessMask = 0;
	lDependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	lDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo lRenderPassInfo{};
	lRenderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	lRenderPassInfo.attachmentCount = 1;
	lRenderPassInfo.pAttachments	= &lColorAttachment;
	lRenderPassInfo.subpassCount	= 1;
	lRenderPassInfo.pSubpasses		= &lSubpass;
	lRenderPassInfo.dependencyCount = 1;
	lRenderPassInfo.pDependencies	= &lDependency;

	RESULT_CONDITION_ENSURE(vkCreateRenderPass(mDevice, &lRenderPassInfo, &mAllocatorCallbacks, &mRenderPass) ==
								VK_SUCCESS,
							VulkanFailedToCreateRenderPass);

	RESULT_OK();
}

INLINE void Manager::CreateGraphicsPipeline(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	const auto&									   lAssetPath = Paths::AssetDir(mAllocator) + "/Shaders/Basic.shader";
	eastl::basic_string<char, default_allocator_t> lShaderBufferData{mAllocator};
	RESULT_ENSURE_CALL(Io::File::ReadAllText(lShaderBufferData, lAssetPath.c_str(), RESULT_ARG_PASS));

	shader_compile_data_t lBasicShaderVertexData{mAllocator};
	shader_compile_data_t lBasicShaderFragmentData{mAllocator};

	RESULT_ENSURE_CALL(lBasicShaderVertexData = mShaderManager.Compile<EShaderKind::eVertex>(
						   lShaderBufferData, "Basic vertex shader", RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(lBasicShaderFragmentData = mShaderManager.Compile<EShaderKind::eFragment>(
						   lShaderBufferData, "Basic fragment shader", RESULT_ARG_PASS));

	RESULT_ENSURE_CALL(VkShaderModule lBasicVertShaderModule =
						   CreateShaderModule(lBasicShaderVertexData, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(VkShaderModule lBasicFragShaderModule =
						   CreateShaderModule(lBasicShaderFragmentData, RESULT_ARG_PASS));

	VkPipelineShaderStageCreateInfo lVertShaderStageInfo{};
	lVertShaderStageInfo.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	lVertShaderStageInfo.stage	= VK_SHADER_STAGE_VERTEX_BIT;
	lVertShaderStageInfo.module = lBasicVertShaderModule;
	lVertShaderStageInfo.pName	= "main";

	VkPipelineShaderStageCreateInfo lFragShaderStageInfo{};
	lFragShaderStageInfo.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	lFragShaderStageInfo.stage	= VK_SHADER_STAGE_FRAGMENT_BIT;
	lFragShaderStageInfo.module = lBasicFragShaderModule;
	lFragShaderStageInfo.pName	= "main";

	VkPipelineShaderStageCreateInfo lShaderStages[] = {lVertShaderStageInfo, lFragShaderStageInfo};

	VkVertexInputBindingDescription lVertexInputBindingDesc{};
	lVertexInputBindingDesc.binding	  = 0;
	lVertexInputBindingDesc.stride	  = sizeof(Vertex);
	lVertexInputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	eastl::array<VkVertexInputAttributeDescription, 2> lVertexAttrDescs{};

	// Position attribute
	lVertexAttrDescs[0].binding	 = 0;
	lVertexAttrDescs[0].location = 0;
	lVertexAttrDescs[0].format	 = VK_FORMAT_R32G32B32_SFLOAT; // Format the data will take (helps define size of data)
	lVertexAttrDescs[0].offset	 = offsetof(Vertex, Position);

	// Color attribute
	lVertexAttrDescs[1].binding = 0;
	lVertexAttrDescs[1].location = 1;
	lVertexAttrDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	lVertexAttrDescs[1].offset = offsetof(Vertex, Color);

	VkPipelineVertexInputStateCreateInfo lVertexInputInfo{};
	lVertexInputInfo.sType							 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	lVertexInputInfo.vertexBindingDescriptionCount	 = 1;
	lVertexInputInfo.pVertexBindingDescriptions		 = &lVertexInputBindingDesc;
	lVertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(lVertexAttrDescs.size());
	lVertexInputInfo.pVertexAttributeDescriptions	 = lVertexAttrDescs.data();

	VkPipelineInputAssemblyStateCreateInfo lInputAssembly{};
	lInputAssembly.sType				  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	lInputAssembly.topology				  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	lInputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo lViewportState{};
	lViewportState.sType		 = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	lViewportState.viewportCount = 1;
	lViewportState.scissorCount	 = 1;

	VkPipelineRasterizationStateCreateInfo lRasterizer{};
	lRasterizer.sType					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	lRasterizer.depthClampEnable		= VK_FALSE;
	lRasterizer.rasterizerDiscardEnable = VK_FALSE;
	lRasterizer.polygonMode				= VK_POLYGON_MODE_FILL;
	lRasterizer.lineWidth				= 1.0f;
	lRasterizer.cullMode				= VK_CULL_MODE_BACK_BIT;
	lRasterizer.frontFace				= VK_FRONT_FACE_CLOCKWISE;
	lRasterizer.depthBiasEnable			= VK_FALSE;

	VkPipelineMultisampleStateCreateInfo lMultisampling{};
	lMultisampling.sType				= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	lMultisampling.sampleShadingEnable	= VK_FALSE;
	lMultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState lColorBlendAttachment{};
	lColorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	lColorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo lColorBlending{};
	lColorBlending.sType			 = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	lColorBlending.logicOpEnable	 = VK_FALSE;
	lColorBlending.logicOp			 = VK_LOGIC_OP_COPY;
	lColorBlending.attachmentCount	 = 1;
	lColorBlending.pAttachments		 = &lColorBlendAttachment;
	lColorBlending.blendConstants[0] = 0.0f;
	lColorBlending.blendConstants[1] = 0.0f;
	lColorBlending.blendConstants[2] = 0.0f;
	lColorBlending.blendConstants[3] = 0.0f;

	VkDynamicState					 lDynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo lDynamicState{};
	lDynamicState.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	lDynamicState.dynamicStateCount = sizeof lDynamicStates / sizeof(VkDynamicState);
	lDynamicState.pDynamicStates	= lDynamicStates;

	VkPipelineLayoutCreateInfo lPipelineLayoutInfo{};
	lPipelineLayoutInfo.sType				   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	lPipelineLayoutInfo.setLayoutCount		   = 0;
	lPipelineLayoutInfo.pushConstantRangeCount = 0;

	RESULT_CONDITION_ENSURE(
		vkCreatePipelineLayout(mDevice, &lPipelineLayoutInfo, &mAllocatorCallbacks, &mPipelineLayout) == VK_SUCCESS,
		VulkanFailedToCreateGraphicsPipeline);

	VkGraphicsPipelineCreateInfo lPipelineInfo{};
	lPipelineInfo.sType				  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	lPipelineInfo.stageCount		  = 2;
	lPipelineInfo.pStages			  = lShaderStages;
	lPipelineInfo.pVertexInputState	  = &lVertexInputInfo;
	lPipelineInfo.pInputAssemblyState = &lInputAssembly;
	lPipelineInfo.pViewportState	  = &lViewportState;
	lPipelineInfo.pRasterizationState = &lRasterizer;
	lPipelineInfo.pMultisampleState	  = &lMultisampling;
	lPipelineInfo.pColorBlendState	  = &lColorBlending;
	lPipelineInfo.pDynamicState		  = &lDynamicState;
	lPipelineInfo.layout			  = mPipelineLayout;
	lPipelineInfo.renderPass		  = mRenderPass;
	lPipelineInfo.subpass			  = 0;
	lPipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

	RESULT_CONDITION_ENSURE(vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &lPipelineInfo, &mAllocatorCallbacks,
													  &mGraphicsPipeline) == VK_SUCCESS,
							VulkanFailedToCreateGraphicsPipeline);

	vkDestroyShaderModule(mDevice, lBasicVertShaderModule, &mAllocatorCallbacks);
	vkDestroyShaderModule(mDevice, lBasicFragShaderModule, &mAllocatorCallbacks);

	RESULT_OK();
}

INLINE void Manager::CreateFramebuffers(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	mSwapchainFramebuffers.Resize(mSwapchainImageViews.Size());

	for (size_t i = 0; i < mSwapchainFramebuffers.Size(); ++i)
	{
		const VkImageView lAttachments[] = {mSwapchainImageViews[i]};

		VkFramebufferCreateInfo lFramebufferInfo{};
		lFramebufferInfo.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		lFramebufferInfo.renderPass		 = mRenderPass;
		lFramebufferInfo.attachmentCount = 1;
		lFramebufferInfo.pAttachments	 = lAttachments;
		lFramebufferInfo.width			 = mSwapchainExtent.width;
		lFramebufferInfo.height			 = mSwapchainExtent.height;
		lFramebufferInfo.layers			 = 1;

		RESULT_CONDITION_ENSURE(vkCreateFramebuffer(mDevice, &lFramebufferInfo, &mAllocatorCallbacks,
													&mSwapchainFramebuffers[i]) == VK_SUCCESS,
								VulkanFailedToCreateFramebuffer);
	}

	RESULT_OK();
}

INLINE VkShaderModule Manager::CreateShaderModule(const shader_compile_data_t& Data, RESULT_PARAM_IMPL) const
{
	VkShaderModule lModule{};
	RESULT_ENSURE_LAST({});

	VkShaderModuleCreateInfo lCreateInfo{};
	lCreateInfo.sType	 = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	lCreateInfo.codeSize = Data.Size();
	lCreateInfo.pCode	 = reinterpret_cast<const uint32_t*>(Data.Data());

	RESULT_CONDITION_ENSURE(vkCreateShaderModule(mDevice, &lCreateInfo, &mAllocatorCallbacks, &lModule) == VK_SUCCESS,
							VulkanFailedToCreateShaderModule, {});

	RESULT_OK();
	return lModule;
}

INLINE void Manager::CreateCommandPool(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	QueueFamilyIndices		lQueueFamilyIndices = FindQueueFamilies(mPhysicalDevice);
	VkCommandPoolCreateInfo lPoolInfo{};
	lPoolInfo.sType			   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	lPoolInfo.flags			   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	lPoolInfo.queueFamilyIndex = lQueueFamilyIndices.GraphicsFamily.value();

	RESULT_CONDITION_ENSURE(vkCreateCommandPool(mDevice, &lPoolInfo, &mAllocatorCallbacks, &mCommandPool) == VK_SUCCESS,
							VulkanFailedToCreateCommandPool);

	RESULT_OK();
}

INLINE void Manager::CreateCommandBuffers(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	mCommandBuffers.Resize(mMaxFrames);

	VkCommandBufferAllocateInfo lAllocInfo{};
	lAllocInfo.sType			  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	lAllocInfo.commandPool		  = mCommandPool;
	lAllocInfo.level			  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	lAllocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.Size());

	RESULT_CONDITION_ENSURE(vkAllocateCommandBuffers(mDevice, &lAllocInfo, mCommandBuffers.Data()) == VK_SUCCESS,
							VulkanFailedToCreateCommandBuffer);

	RESULT_OK();
}

INLINE void Manager::RecordCommandBuffer(const VkCommandBuffer CommandBuffer, const uint32_t ImageIndex,
										 RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	VkCommandBufferBeginInfo lBeginInfo{};
	lBeginInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	lBeginInfo.flags			= 0;	   // Optional
	lBeginInfo.pInheritanceInfo = nullptr; // Optional

	RESULT_CONDITION_ENSURE(vkBeginCommandBuffer(CommandBuffer, &lBeginInfo) == VK_SUCCESS,
							VulkanFailedToBeginCommandBuffer);

	VkRenderPassBeginInfo lRenderPassInfo{};
	lRenderPassInfo.sType			  = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	lRenderPassInfo.renderPass		  = mRenderPass;
	lRenderPassInfo.framebuffer		  = mSwapchainFramebuffers[ImageIndex];
	lRenderPassInfo.renderArea.offset = {0, 0};
	lRenderPassInfo.renderArea.extent = mSwapchainExtent;

	constexpr VkClearValue lClearColor = {{{0.f, 0.f, 0.f, 1.f}}};
	lRenderPassInfo.clearValueCount	   = 1;
	lRenderPassInfo.pClearValues	   = &lClearColor;

	vkCmdBeginRenderPass(CommandBuffer, &lRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

	VkBuffer	 lVertexBuffers[] = {mMesh.GetVertexBuffer()};
	VkDeviceSize lOffsets[]		  = {0};
	vkCmdBindVertexBuffers(CommandBuffer, 0, 1, lVertexBuffers, lOffsets);

	VkViewport lViewport{};
	lViewport.x		   = 0.0f;
	lViewport.y		   = 0.0f;
	lViewport.width	   = static_cast<float>(mSwapchainExtent.width);
	lViewport.height   = static_cast<float>(mSwapchainExtent.height);
	lViewport.minDepth = 0.0f;
	lViewport.maxDepth = 1.0f;
	vkCmdSetViewport(CommandBuffer, 0, 1, &lViewport);

	VkRect2D lScissor{};
	lScissor.offset = {0, 0};
	lScissor.extent = mSwapchainExtent;
	vkCmdSetScissor(CommandBuffer, 0, 1, &lScissor);

	vkCmdDraw(CommandBuffer, mMesh.GetVertexSize(), 1, 0, 0);

	// ImDrawData* main_draw_data = ImGui::GetDrawData();

	// Record dear imgui primitives into command buffer
	// ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

	vkCmdEndRenderPass(CommandBuffer);

	RESULT_CONDITION_ENSURE(vkEndCommandBuffer(CommandBuffer) == VK_SUCCESS, VulkanFailedToEndCommandBuffer);

	RESULT_OK();
}

INLINE void Manager::CreateSyncObjects(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	mImageAvailableSemaphores.Resize(mMaxFrames);
	mRenderFinishedSemaphores.Resize(mMaxFrames);
	mInFlightFences.Resize(mMaxFrames);

	VkSemaphoreCreateInfo lSemaphoreInfo{};
	lSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo lFenceInfo{};
	lFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	lFenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (uint32_t i = 0; i < mMaxFrames; ++i)
	{
		RESULT_CONDITION_ENSURE(vkCreateSemaphore(mDevice, &lSemaphoreInfo, &mAllocatorCallbacks,
												  &mImageAvailableSemaphores[i]) == VK_SUCCESS,
								VulkanFailedToCreateSemaphore);

		RESULT_CONDITION_ENSURE(vkCreateSemaphore(mDevice, &lSemaphoreInfo, &mAllocatorCallbacks,
												  &mRenderFinishedSemaphores[i]) == VK_SUCCESS,
								VulkanFailedToCreateSemaphore);

		RESULT_CONDITION_ENSURE(vkCreateFence(mDevice, &lFenceInfo, &mAllocatorCallbacks, &mInFlightFences[i]) ==
									VK_SUCCESS,
								VulkanFailedToCreateFence);
	}

	RESULT_OK();
}

INLINE void Manager::DestroySwapchain(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	for (const auto lSwapchainFramebuffer: mSwapchainFramebuffers)
	{
		vkDestroyFramebuffer(mDevice, lSwapchainFramebuffer, &mAllocatorCallbacks);
	}

	for (const auto lSwapchainImageView: mSwapchainImageViews)
	{
		vkDestroyImageView(mDevice, lSwapchainImageView, &mAllocatorCallbacks);
	}

	vkDestroySwapchainKHR(mDevice, mSwapchain, &mAllocatorCallbacks);
	RESULT_OK();
}

INLINE void Manager::DestroySyncObjects(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	for (uint32_t lI = 0; lI < mMaxFrames; ++lI)
	{
		vkDestroySemaphore(mDevice, mImageAvailableSemaphores[lI], &mAllocatorCallbacks);
		vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[lI], &mAllocatorCallbacks);
		vkDestroyFence(mDevice, mInFlightFences[lI], &mAllocatorCallbacks);
	}

	RESULT_OK();
}

INLINE void Manager::RecreateSwapchain(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	vkDeviceWaitIdle(mDevice);

	if (mWindow.IsMinimized())
	{
		RESULT_OK();
		return;
	}

	RESULT_ENSURE_CALL(DestroySwapchain(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateSwapchain(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateImageViews(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateFramebuffers(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(RecreateSyncObjects(RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE void Manager::RecreateSyncObjects(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(DestroySyncObjects(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateSyncObjects(RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE bool Manager::IsDeviceSuitable(const VkPhysicalDevice Device) const
{
	const QueueFamilyIndices lIndices			  = FindQueueFamilies(Device);
	const bool				 lExtensionsSupported = CheckDeviceExtensionSupport(Device);

	bool lSwapChainAdequate = false;
	if (lExtensionsSupported)
	{
		const SwapchainSupportDetails lSwapChainSupport = QuerySwapchainSupport(Device);
		lSwapChainAdequate = !lSwapChainSupport.Formats.IsEmpty() && !lSwapChainSupport.PresentModes.IsEmpty();
	}

	return lIndices.IsComplete() && lExtensionsSupported && lSwapChainAdequate;
}

INLINE Manager::QueueFamilyIndices Manager::FindQueueFamilies(const VkPhysicalDevice Device) const
{
	QueueFamilyIndices lIndices;

	uint32_t lQueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(Device, &lQueueFamilyCount, nullptr);

	RawBuffer<VkQueueFamilyProperties, default_allocator_t> lQueueFamilies{lQueueFamilyCount, mAllocator};
	vkGetPhysicalDeviceQueueFamilyProperties(Device, &lQueueFamilyCount, lQueueFamilies.Data());

	int lI = 0;
	for (const auto& lQueueFamily: lQueueFamilies)
	{
		if (lQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			lIndices.GraphicsFamily = lI;
		}

		VkBool32 lPresentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(Device, lI, mSurface, &lPresentSupport);

		if (lPresentSupport)
		{
			lIndices.PresentFamily = lI;
		}

		if (lIndices.IsComplete())
		{
			break;
		}

		++lI;
	}

	return lIndices;
}

INLINE bool Manager::CheckDeviceExtensionSupport(const VkPhysicalDevice Device) const
{
	uint32_t lExtensionCount;
	vkEnumerateDeviceExtensionProperties(Device, nullptr, &lExtensionCount, nullptr);

	RawBuffer<VkExtensionProperties, default_allocator_t> lAvailableExtensions{lExtensionCount, mAllocator};
	vkEnumerateDeviceExtensionProperties(Device, nullptr, &lExtensionCount, lAvailableExtensions.Data());

	eastl::set<eastl::string> lRequiredExtensions(mDeviceExtensions.begin(), mDeviceExtensions.end());

	for (const auto& lExtension: lAvailableExtensions)
	{
		lRequiredExtensions.erase(lExtension.extensionName);
	}

	return lRequiredExtensions.empty();
}

INLINE Manager::SwapchainSupportDetails Manager::QuerySwapchainSupport(const VkPhysicalDevice Device) const
{
	SwapchainSupportDetails lDetails;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, mSurface, &lDetails.Capabilities);

	uint32_t lFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(Device, mSurface, &lFormatCount, nullptr);

	if (lFormatCount != 0)
	{
		lDetails.Formats.Resize(lFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(Device, mSurface, &lFormatCount, lDetails.Formats.Data());
	}

	uint32_t lPresentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(Device, mSurface, &lPresentModeCount, nullptr);

	if (lPresentModeCount != 0)
	{
		lDetails.PresentModes.Resize(lPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(Device, mSurface, &lPresentModeCount, lDetails.PresentModes.Data());
	}

	return lDetails;
}

INLINE VkSurfaceFormatKHR
Manager::ChooseSwapSurfaceFormat(const RawBuffer<VkSurfaceFormatKHR, default_allocator_t>& AvailableFormats)
{
	for (const auto& lAvailableFormat: AvailableFormats)
	{
		if (lAvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			lAvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return lAvailableFormat;
		}
	}

	return AvailableFormats.At(0);
}

INLINE VkPresentModeKHR
Manager::ChooseSwapPresentMode(const RawBuffer<VkPresentModeKHR, default_allocator_t>& AvailablePresentModes)
{
	for (const auto& lAvailablePresentMode: AvailablePresentModes)
	{
		if (lAvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return lAvailablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

INLINE VkExtent2D Manager::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilities) const
{
	if (Capabilities.currentExtent.width != UINT32_MAX)
	{
		return Capabilities.currentExtent;
	}
	const auto lWindowSize = mWindow.GetSize();

	VkExtent2D lActualExtent = {lWindowSize.x, lWindowSize.y};

	lActualExtent.width =
		glm::clamp(lActualExtent.width, Capabilities.minImageExtent.width, Capabilities.maxImageExtent.width);
	lActualExtent.height =
		glm::clamp(lActualExtent.height, Capabilities.minImageExtent.height, Capabilities.maxImageExtent.height);

	return lActualExtent;
}

INLINE void* Manager::Allocation(void* UserData, const size_t Size, const size_t Alignment,
								 VkSystemAllocationScope AllocationScope)
{
	// LOGC(Verbose, RenderApiVulkan, "Vulkan Allocation: %llu", Size);
	const auto lAllocator = static_cast<default_allocator_t*>(UserData);
	return lAllocator->allocate(Size, Alignment, 0, 0);
}

INLINE void* Manager::Reallocation(void* UserData, void* Original, size_t Size, size_t Alignment,
								   VkSystemAllocationScope AllocationScope)
{
	// LOGC(Verbose, RenderApiVulkan, "Vulkan Reallocation: %llu", Size);
	const auto lAllocator = static_cast<default_allocator_t*>(UserData);
	return lAllocator->reallocate(Original, Size, Alignment, 0, 0);
}

INLINE void Manager::Free(void* UserData, void* Memory)
{
	// LOGC(Verbose, RenderApiVulkan, "Vulkan Free: %p", Memory);
	const auto lAllocator = static_cast<default_allocator_t*>(UserData);
	lAllocator->deallocate(Memory, 0);
}

INLINE void Manager::InternalAllocationNotification(void* UserData, const size_t Size,
													VkInternalAllocationType AllocationType,
													VkSystemAllocationScope	 AllocationScope)
{
	// const auto lAllocator = static_cast<default_allocator_t*>(UserData);
	LOGC(Verbose, RenderApiVulkan, "Vulkan Allocation: %llu", Size);
}

INLINE void Manager::InternalFreeNotification(void* UserData, const size_t Size,
											  VkInternalAllocationType AllocationType,
											  VkSystemAllocationScope  AllocationScope)
{
	LOGC(Verbose, RenderApiVulkan, "Vulkan Free: %llu", Size);
}

#if VK_ENABLE_VALIDATION_LAYER
INLINE void Manager::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& CreateInfo)
{
	CreateInfo				   = {};
	CreateInfo.sType		   = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	CreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	CreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	CreateInfo.pfnUserCallback = DebugCallback;
}

INLINE void Manager::SetupDebugMessenger(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	VkDebugUtilsMessengerCreateInfoEXT lCreateInfo;
	PopulateDebugMessengerCreateInfo(lCreateInfo);
	VkResult lResult;
	RESULT_CONDITION_ENSURE(
		(lResult = CreateDebugUtilsMessengerExt(mInstance, &lCreateInfo, nullptr, &mDebugMessenger)) == VK_SUCCESS,
		VulkanFailedToSetupDebugMessenger);
	RESULT_OK();
}

INLINE bool Manager::CheckValidationLayoutSupport() const
{
	uint32_t lLayerCount;
	vkEnumerateInstanceLayerProperties(&lLayerCount, nullptr);

	RawBuffer<VkLayerProperties> lAvailableLayers{lLayerCount, DEBUG_NAME_VAL("Render::VulkanManager Vulkan")};
	vkEnumerateInstanceLayerProperties(&lLayerCount, lAvailableLayers.Data());

	for (const char* lLayerName: mValidationLayers)
	{
		bool lLayerFound = false;

		for (const auto& lLayerProperties: lAvailableLayers)
		{
			if (strcmp(lLayerName, lLayerProperties.layerName) == 0)
			{
				lLayerFound = true;
				break;
			}
		}

		if (!lLayerFound)
		{
			return false;
		}
	}

	return true;
}

INLINE VkResult Manager::CreateDebugUtilsMessengerExt(const VkInstance							Instance,
													  const VkDebugUtilsMessengerCreateInfoEXT* CreateInfo,
													  const VkAllocationCallbacks*				Allocator,
													  VkDebugUtilsMessengerEXT*					DebugMessenger)
{
	if (const auto lFunc = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT"));
		lFunc != nullptr)
	{
		return lFunc(Instance, CreateInfo, Allocator, DebugMessenger);
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

INLINE void Manager::DestroyDebugUtilsMessengerExt(const VkInstance				  Instance,
												   const VkDebugUtilsMessengerEXT DebugMessenger,
												   const VkAllocationCallbacks*	  Allocator)
{
	if (const auto lFunc = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT"));
		lFunc != nullptr)
	{
		lFunc(Instance, DebugMessenger, Allocator);
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL Manager::DebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
													  VkDebugUtilsMessageTypeFlagsEXT			   MessageType,
													  const VkDebugUtilsMessengerCallbackDataEXT*  CallbackData,
													  void*										   UserData)
{
	if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		LOGC(Info, RenderApiVulkan, "%s", CallbackData->pMessage);
	}
	else if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		LOGC(Error, RenderApiVulkan, "%s", CallbackData->pMessage);
	}
	else if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		LOGC(Warning, RenderApiVulkan, "%s", CallbackData->pMessage);
	}
	else if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		LOGC(Verbose, RenderApiVulkan, "%s", CallbackData->pMessage);
	}
	return VK_FALSE;
}

#endif

INLINE Mesh::Mesh(Manager* Manager, const VkPhysicalDevice PhysicalDevice, const VkDevice Device,
				  eastl::vector<Vertex, default_allocator_t>& Vertices, RESULT_PARAM_IMPL)
	: mManager{Manager}, mVertexSize{static_cast<uint32_t>(Vertices.size())},
	  mPhysicalDevice{PhysicalDevice}, mDevice{Device}
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(CreateVertexBuffer(Vertices, RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE Mesh::~Mesh()
{
	if(mVertexBuffer && mVertexBufferMemory)
	{
		DestroyVertexBuffer();
	}
}

INLINE Mesh::Mesh(Mesh&& Other) NOEXCEPT: mManager{Other.mManager},
										  mVertexSize{Other.mVertexSize},
										  mVertexBuffer{Other.mVertexBuffer},
										  mVertexBufferMemory{Other.mVertexBufferMemory},
										  mPhysicalDevice{Other.mPhysicalDevice},
										  mDevice{Other.mDevice}
{
	Other.mManager			= nullptr;
	Other.mVertexSize			= 0;
	Other.mVertexBuffer		= nullptr;
	Other.mVertexBufferMemory = nullptr;
	Other.mPhysicalDevice		= nullptr;
	Other.mDevice				= nullptr;
}

INLINE Mesh& Mesh::operator=(Mesh&& Other) NOEXCEPT
{
	mManager			= Other.mManager;
	mVertexSize			= Other.mVertexSize;
	mVertexBuffer		= Other.mVertexBuffer;
	mVertexBufferMemory = Other.mVertexBufferMemory;
	mPhysicalDevice		= Other.mPhysicalDevice;
	mDevice				= Other.mDevice;
	Other.mManager			= nullptr;
	Other.mVertexSize			= 0;
	Other.mVertexBuffer		= nullptr;
	Other.mVertexBufferMemory = nullptr;
	Other.mPhysicalDevice		= nullptr;
	Other.mDevice				= nullptr;
	return *this;
}

INLINE Mesh Mesh::Square(Manager* Manager, const VkPhysicalDevice PhysicalDevice, const VkDevice Device, RESULT_PARAM_IMPL)
{
	Mesh lMesh{};
	eastl::vector<Vertex, default_allocator_t> lVertices
	{
		{{0.5f, -0.5f, 0.f}, {1.f, 0.f, 0.f}},
		{{0.5f, 0.5f, 0.f}, {0.f, 1.f, 0.f}},
		{{-0.5f, 0.5f, 0.f}, {0.f, 0.f, 1.f}},

		{{-0.5f, 0.5f, 0.f}, {0.f, 0.f, 1.f}},
		{{-0.5f, -0.5f, 0.f}, {0.f, 1.f, 0.f}},
		{{0.5f, -0.5f, 0.f}, {1.f, 0.f, 0.f}}
	};
	RESULT_ENSURE_CALL(lMesh = Mesh(Manager, PhysicalDevice, Device, lVertices, RESULT_ARG_PASS), lMesh);
	return lMesh;
}

INLINE void Mesh::DestroyVertexBuffer(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	vkDestroyBuffer(mDevice, mVertexBuffer, &mManager->mAllocatorCallbacks);
	vkFreeMemory(mDevice, mVertexBufferMemory, &mManager->mAllocatorCallbacks);
	mVertexBuffer		= nullptr;
	mVertexBufferMemory = nullptr;
	RESULT_OK();
}

INLINE uint32_t Mesh::GetVertexSize() const
{
	return mVertexSize;
}

INLINE VkBuffer Mesh::GetVertexBuffer() const
{
	return mVertexBuffer;
}

INLINE void Mesh::CreateVertexBuffer(eastl::vector<Vertex, default_allocator_t>& Vertices, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	VkBufferCreateInfo lBufferCreateInfo{};
	lBufferCreateInfo.sType		  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	lBufferCreateInfo.size		  = sizeof(Vertex) * mVertexSize;
	lBufferCreateInfo.usage		  = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	lBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	RESULT_CONDITION_ENSURE(
		vkCreateBuffer(mDevice, &lBufferCreateInfo, &mManager->mAllocatorCallbacks, &mVertexBuffer) == VK_SUCCESS,
		VulkanFailedToCreateBuffer);

	// Get buffer memory requirements
	VkMemoryRequirements lMemoryRequirements{};
	vkGetBufferMemoryRequirements(mDevice, mVertexBuffer, &lMemoryRequirements);

	// Allocate memory to buffer
	VkMemoryAllocateInfo lMemoryAllocateInfo{};
	lMemoryAllocateInfo.sType		   = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	lMemoryAllocateInfo.allocationSize = lMemoryRequirements.size;

	// Find memory type index
	RESULT_ENSURE_CALL(lMemoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndex(
						   lMemoryRequirements.memoryTypeBits,
						   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						   RESULT_ARG_PASS));

	RESULT_CONDITION_ENSURE(vkAllocateMemory(mDevice, &lMemoryAllocateInfo, &mManager->mAllocatorCallbacks,
											 &mVertexBufferMemory) == VK_SUCCESS,
							VulkanFailedToAllocateMemory);

	// Bind allocated memory to buffer
	vkBindBufferMemory(mDevice, mVertexBuffer, mVertexBufferMemory, 0);

	// Map memory to vertex buffer
	void* lData{}; // 1. Create pointer to RAM
	vkMapMemory(mDevice, mVertexBufferMemory, 0, lBufferCreateInfo.size, 0,
				&lData);									// 2. "Map" the device memory to raw pointer in ram.
	memcpy(lData, Vertices.data(), lBufferCreateInfo.size); // 3. Copy memory from vertices to mapped data pointer.
	vkUnmapMemory(mDevice, mVertexBufferMemory);			// 4. Unmap device memory.

	RESULT_OK();
}

INLINE uint32_t Mesh::FindMemoryTypeIndex(const uint32_t AllowedTypes, const VkMemoryPropertyFlags PropertyFlags,
										  RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST({});
	RESULT_CONDITION_ENSURE(AllowedTypes && static_cast<uint32_t>(PropertyFlags), ZeroSize, {});

	VkPhysicalDeviceMemoryProperties lMemoryProperties{};
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &lMemoryProperties);

	for (uint32_t lI = 0u; lI < lMemoryProperties.memoryTypeCount; ++lI)
	{
		if (AllowedTypes & 1 << lI &&
			(lMemoryProperties.memoryTypes[lI].propertyFlags & PropertyFlags) == PropertyFlags)
		{
			return lI;
		}
	}

	RESULT_OK();
	return uint32_t{};
}

} // namespace Render::Api

CLASS_VALIDATION(Render::Api::Mesh);
CLASS_VALIDATION(Render::Api::Manager);

#endif
