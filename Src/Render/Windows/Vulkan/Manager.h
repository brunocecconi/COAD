
#if VULKAN_ENABLED

#undef VK_ENABLE_VALIDATION_LAYER
#ifndef NDEBUG
#define VK_ENABLE_VALIDATION_LAYER 1
#else
#define VK_ENABLE_VALIDATION_LAYER 0
#endif

LOG_DEFINE(RenderApiVulkan);

#include <EASTL/optional.h>

#include "Render/Windows/Vulkan/Allocator.h"
#include "Render/Windows/Vulkan/Utils.h"
#include "Render/Windows/Vulkan/ShaderManager.h"
#include "Render/Windows/Vulkan/Image.h"
#include "Render/Windows/Vulkan/Mesh.h"
#include "Render/Windows/Vulkan/Swapchain.h"

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

	void		   CreateInstance(RESULT_PARAM_DEFINE);
	void		   CreateSurface(RESULT_PARAM_DEFINE);
	void		   PickPhysicalDevice(RESULT_PARAM_DEFINE);
	void		   CreateLogicalDevice(RESULT_PARAM_DEFINE);
	void		   CreateSwapchain(RESULT_PARAM_DEFINE);
	void		   CreateImageViews(RESULT_PARAM_DEFINE);
	void		   CreateRenderPass(RESULT_PARAM_DEFINE);
	void		   CreateDescriptorSetLayout(RESULT_PARAM_DEFINE);
	void		   CreatePushConstantRange(RESULT_PARAM_DEFINE);
	void		   CreateGraphicsPipeline(RESULT_PARAM_DEFINE);
	void		   CreateFramebuffers(RESULT_PARAM_DEFINE);
	VkShaderModule CreateShaderModule(const shader_compile_data_t& Data, RESULT_PARAM_DEFINE) const;
	void		   CreateUniformBuffers(RESULT_PARAM_DEFINE);
	void		   CreateDescriptorPool(RESULT_PARAM_DEFINE);
	void		   CreateDescriptorSets(RESULT_PARAM_DEFINE);

	void UpdateUniformBuffers(uint32_t ImageIndex, RESULT_PARAM_DEFINE);

	void CreateCommandPool(RESULT_PARAM_DEFINE);
	void CreateCommandBuffers(RESULT_PARAM_DEFINE);
	void RecordCommandBuffer(VkCommandBuffer CommandBuffer, uint32_t ImageIndex, RESULT_PARAM_DEFINE);
	void CreateSyncObjects(RESULT_PARAM_DEFINE);

	void DestroySwapchain(RESULT_PARAM_DEFINE);
	void DestroySyncObjects(RESULT_PARAM_DEFINE);
	void RecreateSwapchain(RESULT_PARAM_DEFINE);
	void RecreateSyncObjects(RESULT_PARAM_DEFINE);

private:
	void AllocateDynamicBufferTransferSpace(RESULT_PARAM_DEFINE);
	void UpdateModel(uint32_t Id, glm::mat4 Model, RESULT_PARAM_DEFINE);

private:
	NODISCARD bool				 IsDeviceSuitable(VkPhysicalDevice Device) const;
	NODISCARD QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice Device) const;
	NODISCARD bool				 CheckDeviceExtensionSupport(VkPhysicalDevice Device) const;
	NODISCARD Swapchain::SupportDetails QuerySwapchainSupport(VkPhysicalDevice Device) const;
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
#ifndef RELEASE
	ShaderManager mShaderManager{};
#endif

private:
	default_allocator_t	  mAllocator{};
	VkAllocationCallbacks mAllocationCallbacks{};

private:
	uint32_t		 mMaxObjects = 2u;
	uint32_t		 mMaxFrames	 = 3u, mCurrentFrame{};
	VkInstance		 mInstance{};
	VkPhysicalDevice mPhysicalDevice{};
	VkDevice		 mDevice{};
	VkQueue			 mGraphicsQueue{}, mPresentQueue{};
	Swapchain		 mSwapchain{mAllocator};
	VkRenderPass	 mRenderPass{};
	VkPipelineLayout mGraphicsPipelineLayout{};
	VkPipeline		 mGraphicsPipeline{};

	VkCommandPool									mCommandPool{};
	RawBuffer<VkCommandBuffer, default_allocator_t> mCommandBuffers{mAllocator};
	RawBuffer<VkSemaphore, default_allocator_t>		mImageAvailableSemaphores{mAllocator};
	RawBuffer<VkSemaphore, default_allocator_t>		mRenderFinishedSemaphores{mAllocator};
	RawBuffer<VkFence, default_allocator_t>			mInFlightFences{mAllocator};

	VkDescriptorSetLayout								mDescriptorSetLayout{};
	VkPushConstantRange									mPushConstantRange{};
	VkDescriptorPool									mDescriptorPool{};
	eastl::vector<VkDescriptorSet, default_allocator_t> mDescriptorSets{};

	eastl::vector<VkBuffer, default_allocator_t>	   mScreenPropertiesUniformBuffer{};
	eastl::vector<VkDeviceMemory, default_allocator_t> mScreenPropertiesUniformBufferMemory{};
	ScreenProperties								   mScreenProperties{};

private:
	bool mMarkDirtyFramebufferSize{};

private:
	Mesh mScreenMesh{};

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
	RESULT_ENSURE_CALL(CreateSwapchain(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateImageViews(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateRenderPass(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateDescriptorSetLayout(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreatePushConstantRange(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateGraphicsPipeline(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateFramebuffers(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateCommandPool(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateCommandBuffers(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(AllocateDynamicBufferTransferSpace(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateSyncObjects(RESULT_ARG_PASS));

	RESULT_ENSURE_CALL(CreateUniformBuffers(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateDescriptorPool(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateDescriptorSets(RESULT_ARG_PASS));

#if EDITOR
	ImGui_ImplVulkan_InitInfo lInitInfo{};
	lInitInfo.Device		 = mDevice;
	lInitInfo.Instance		 = mInstance;
	lInitInfo.Allocator		 = &mAllocationCallbacks;
	lInitInfo.DescriptorPool = mDescriptorPool;
	lInitInfo.MinImageCount	 = mMaxFrames;
	lInitInfo.ImageCount	 = mMaxFrames;
	lInitInfo.PhysicalDevice = mPhysicalDevice;
	lInitInfo.Queue			 = mGraphicsQueue;
	lInitInfo.MSAASamples	 = VK_SAMPLE_COUNT_1_BIT;
	lInitInfo.QueueFamily	 = FindQueueFamilies(mPhysicalDevice).GraphicsFamily.value();

	Editor::Manager::InitializeInfo lEditorInitInfo{};
	lEditorInitInfo.Window		  = &mWindow;
	lEditorInitInfo.ImguiInitInfo = &lInitInfo;
	lEditorInitInfo.RenderPass	  = mRenderPass;
	lEditorInitInfo.Surface		  = mSwapchain.Surface;
	lEditorInitInfo.Swapchain	  = mSwapchain.Object;

	RESULT_ENSURE_CALL(Editor::Instance().Initialize(lEditorInitInfo, RESULT_ARG_PASS));

	//// Upload Fonts
	//{
	//	// Use any command queue
	//	const VkCommandBuffer lCommandBuffer = mCommandBuffers[mCurrentFrame++];

	//	vkResetCommandPool(mDevice, mCommandPool, 0);
	//	VkCommandBufferBeginInfo lBeginInfo = {};
	//	lBeginInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//	lBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	//	vkBeginCommandBuffer(lCommandBuffer, &lBeginInfo);

	//	ImGui_ImplVulkan_CreateFontsTexture(lCommandBuffer);

	//	VkSubmitInfo lEndInfo		= {};
	//	lEndInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//	lEndInfo.commandBufferCount = 1;
	//	lEndInfo.pCommandBuffers	= &lCommandBuffer;
	//	vkEndCommandBuffer(lCommandBuffer);
	//	vkQueueSubmit(mGraphicsQueue, 1, &lEndInfo, VK_NULL_HANDLE);

	//	vkDeviceWaitIdle(mDevice);
	//	ImGui_ImplVulkan_DestroyFontUploadObjects();
	//}

#endif

	Mesh::Info lMeshInfo{};
	lMeshInfo.PhysicalDevice	 = mPhysicalDevice;
	lMeshInfo.Device			 = mDevice;
	lMeshInfo.AllocatorCallbacks = &mAllocationCallbacks;
	lMeshInfo.TransferQueue		 = mGraphicsQueue;
	lMeshInfo.TransferCmdPool	 = mCommandPool;
	lMeshInfo.Vertices			 = {{{1.f, -1.f}}, {{1.f, 1.f}}, {{-1.f, 1.f}}, {{-1.f, -1.f}}}; // 0,1,2,2,3,0
	lMeshInfo.Indices			 = {{2, 1, 0, 0, 3, 2}};

	RESULT_ENSURE_CALL(mScreenMesh = Mesh(lMeshInfo, RESULT_ARG_PASS));

	RESULT_OK();
}

INLINE void Manager::Update(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

	uint32_t lImageIndex{};
	VkResult lResult = vkAcquireNextImageKHR(mDevice, mSwapchain.Object, UINT64_MAX,
											 mImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &lImageIndex);

	if (lResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RESULT_ENSURE_CALL(RecreateSwapchain(RESULT_ARG_PASS));
		RESULT_OK();
		return;
	}

	RESULT_CONDITION_ENSURE(lResult == VK_SUCCESS || lResult == VK_SUBOPTIMAL_KHR, VulkanFailedToAcquireSwapchainImage);

	vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

	RESULT_ENSURE_CALL(UpdateUniformBuffers(mCurrentFrame, RESULT_ARG_PASS));

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

	const VkSwapchainKHR lSwapChains[] = {mSwapchain.Object};
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

	RESULT_OK();
}

INLINE void Manager::Finalize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	RESULT_CONDITION_ENSURE(mInstance, NullPtr);
	RESULT_CONDITION_ENSURE(mSwapchain.Surface, NullPtr);
	RESULT_CONDITION_ENSURE(mSwapchain.Object, NullPtr);

	vkDeviceWaitIdle(mDevice);

#if EDITOR
	RESULT_ENSURE_CALL(Editor::Instance().Finalize(RESULT_ARG_PASS));
#endif

	vkDestroyDescriptorPool(mDevice, mDescriptorPool, &mAllocationCallbacks);
	vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, &mAllocationCallbacks);

	for (uint32_t lI = 0; lI < mMaxFrames; ++lI)
	{
		Utils::DestroyBuffer(mDevice, mScreenPropertiesUniformBuffer[lI], mScreenPropertiesUniformBufferMemory[lI],
							 &mAllocationCallbacks);
	}

	RESULT_ENSURE_CALL(mScreenMesh.DestroyBuffers(RESULT_ARG_PASS));

	RESULT_ENSURE_CALL(DestroySwapchain(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(DestroySyncObjects(RESULT_ARG_PASS));

	vkDestroyCommandPool(mDevice, mCommandPool, &mAllocationCallbacks);

	vkDestroyPipeline(mDevice, mGraphicsPipeline, &mAllocationCallbacks);
	vkDestroyPipelineLayout(mDevice, mGraphicsPipelineLayout, &mAllocationCallbacks);
	vkDestroyRenderPass(mDevice, mRenderPass, &mAllocationCallbacks);

	vkDestroyDevice(mDevice, &mAllocationCallbacks);

#if VK_ENABLE_VALIDATION_LAYER
	DestroyDebugUtilsMessengerExt(mInstance, mDebugMessenger, &mAllocationCallbacks);
#endif

	vkDestroySurfaceKHR(mInstance, mSwapchain.Surface, &mAllocationCallbacks);
	vkDestroyInstance(mInstance, &mAllocationCallbacks);

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
	LOGC(Info, RenderApiVulkan, "Marking dirty framebuffer size. New size: [%u,%u].", mSwapchain.Extent.width,
		 mSwapchain.Extent.height);
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
	mAllocationCallbacks.pUserData			   = &mAllocator;
	mAllocationCallbacks.pfnAllocation		   = Allocation;
	mAllocationCallbacks.pfnFree			   = Free;
	mAllocationCallbacks.pfnInternalAllocation = InternalAllocationNotification;
	mAllocationCallbacks.pfnInternalFree	   = InternalFreeNotification;
	mAllocationCallbacks.pfnReallocation	   = Reallocation;

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
	RESULT_CONDITION_ENSURE((lResult = vkCreateInstance(&lInstanceCi, &mAllocationCallbacks, &mInstance)) == VK_SUCCESS,
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
	RESULT_CONDITION_ENSURE(
		vkCreateWin32SurfaceKHR(mInstance, &lCreateInfo, &mAllocationCallbacks, &mSwapchain.Surface) == VK_SUCCESS,
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

	// VkPhysicalDeviceProperties lDeviceProperties{};
	// vkGetPhysicalDeviceProperties(mPhysicalDevice, &lDeviceProperties);
	// mMinUniformBufferOffset			  = lDeviceProperties.limits.minUniformBufferOffsetAlignment;
	// mScreenPropertiesUniformAlignment = glm::max(sizeof(ScreenProperties), mMinUniformBufferOffset);
	RESULT_OK();
}

INLINE void Manager::CreateLogicalDevice(RESULT_PARAM_IMPL)
{
	QueueFamilyIndices lIndices = FindQueueFamilies(mPhysicalDevice);

	eastl::vector<VkDeviceQueueCreateInfo, default_allocator_t>		 lQueueCreateInfos{mAllocator};
	eastl::set<uint32_t, eastl::less<uint32_t>, default_allocator_t> lUniqueQueueFamilies = {
		lIndices.GraphicsFamily.value(), lIndices.PresentFamily.value()};
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
	lCreateInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	lCreateInfo.pQueueCreateInfos		= lQueueCreateInfos.data();
	lCreateInfo.queueCreateInfoCount	= 1;
	lCreateInfo.pEnabledFeatures		= &lDeviceFeatures;
	lCreateInfo.enabledExtensionCount	= static_cast<uint32_t>(mDeviceExtensions.size());
	lCreateInfo.ppEnabledExtensionNames = mDeviceExtensions.data();
#if VK_ENABLE_VALIDATION_LAYER
	lCreateInfo.enabledLayerCount	= static_cast<uint32_t>(mValidationLayers.size());
	lCreateInfo.ppEnabledLayerNames = mValidationLayers.data();
#else
	lCreateInfo.enabledLayerCount = 0;
#endif

	RESULT_CONDITION_ENSURE(vkCreateDevice(mPhysicalDevice, &lCreateInfo, &mAllocationCallbacks, &mDevice) ==
								VK_SUCCESS,
							VulkanFailedToCreateDevice);

	vkGetDeviceQueue(mDevice, lIndices.GraphicsFamily.value(), 0, &mGraphicsQueue);
	vkGetDeviceQueue(mDevice, lIndices.PresentFamily.value(), 0, &mPresentQueue);
}

INLINE void Manager::CreateSwapchain(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	const Swapchain::SupportDetails lSwapChainSupport = QuerySwapchainSupport(mPhysicalDevice);

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
	lCreateInfo.surface = mSwapchain.Surface;

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

	RESULT_CONDITION_ENSURE(vkCreateSwapchainKHR(mDevice, &lCreateInfo, &mAllocationCallbacks, &mSwapchain.Object) ==
								VK_SUCCESS,
							VulkanFailedToCreateSwapchain);

	vkGetSwapchainImagesKHR(mDevice, mSwapchain.Object, &lImageCount, nullptr);
	mSwapchain.Images.Resize(lImageCount);
	vkGetSwapchainImagesKHR(mDevice, mSwapchain.Object, &lImageCount, mSwapchain.Images.Data());

	mSwapchain.ImageFormat = lSurfaceFormat.format;
	mSwapchain.Extent	   = lExtent;
	mScreenProperties.Resolution = {mSwapchain.Extent.width, mSwapchain.Extent.height};
	RESULT_OK();
}

INLINE void Manager::CreateImageViews(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	mSwapchain.ImageViews.Resize(mSwapchain.Images.Size());

	for (size_t lI = 0; lI < mSwapchain.Images.Size(); lI++)
	{
		VkImageViewCreateInfo lCreateInfo{};
		lCreateInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		lCreateInfo.image							= mSwapchain.Images[lI];
		lCreateInfo.viewType						= VK_IMAGE_VIEW_TYPE_2D;
		lCreateInfo.format							= mSwapchain.ImageFormat;
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
			vkCreateImageView(mDevice, &lCreateInfo, &mAllocationCallbacks, &mSwapchain.ImageViews[lI]) == VK_SUCCESS,
			VulkanFailedToCreateSwapchainImageView);
	}
	RESULT_OK();
}

INLINE void Manager::CreateRenderPass(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	VkAttachmentDescription lColorAttachment{};
	lColorAttachment.format			= mSwapchain.ImageFormat;
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

	RESULT_CONDITION_ENSURE(vkCreateRenderPass(mDevice, &lRenderPassInfo, &mAllocationCallbacks, &mRenderPass) ==
								VK_SUCCESS,
							VulkanFailedToCreateRenderPass);

	RESULT_OK();
}

INLINE void Manager::CreateDescriptorSetLayout(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	// VkSampler lSamplers[] = {};

	// UboModel binding info
	VkDescriptorSetLayoutBinding lScreenPropertiesLayoutBinding{};
	lScreenPropertiesLayoutBinding.binding			  = 0;
	lScreenPropertiesLayoutBinding.descriptorType	  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	lScreenPropertiesLayoutBinding.descriptorCount	  = 1;
	lScreenPropertiesLayoutBinding.stageFlags		  = VK_SHADER_STAGE_FRAGMENT_BIT;
	lScreenPropertiesLayoutBinding.pImmutableSamplers = nullptr;

	eastl::array lBindings{lScreenPropertiesLayoutBinding};

	VkDescriptorSetLayoutCreateInfo lLayoutCreateInfo{};
	lLayoutCreateInfo.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	lLayoutCreateInfo.bindingCount = static_cast<uint32_t>(lBindings.size());
	lLayoutCreateInfo.pBindings	   = lBindings.data();

	RESULT_CONDITION_ENSURE(vkCreateDescriptorSetLayout(mDevice, &lLayoutCreateInfo, &mAllocationCallbacks,
														&mDescriptorSetLayout) == VK_SUCCESS,
							VulkanFailedToCreateDescriptoSetLayout);

	RESULT_OK();
}

INLINE void Manager::CreatePushConstantRange(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	/*mPushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	mPushConstantRange.offset	  = 0;
	mPushConstantRange.size		  = sizeof(Model);*/
	RESULT_OK();
}

INLINE void Manager::CreateGraphicsPipeline(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	const auto& lAssetPath = Paths::AssetDir(mAllocator) + "/Shaders/NonAccelerationRT.shader";
	eastl::basic_string<char, default_allocator_t> lShaderBufferData{mAllocator};
	RESULT_ENSURE_CALL(Io::File::ReadAllText(lShaderBufferData, lAssetPath.c_str(), RESULT_ARG_PASS));

	shader_compile_data_t lBasicShaderVertexData{mAllocator};
	shader_compile_data_t lBasicShaderFragmentData{mAllocator};

	RESULT_ENSURE_CALL(lBasicShaderVertexData = mShaderManager.Compile<EShaderKind::eVertex>(
						   lShaderBufferData, "Non acceleration ray tracing shader", RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(lBasicShaderFragmentData = mShaderManager.Compile<EShaderKind::eFragment>(
						   lShaderBufferData, "Non acceleration ray tracing shader", RESULT_ARG_PASS));

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

	eastl::array lShaderStages{lVertShaderStageInfo, lFragShaderStageInfo};

	VkVertexInputBindingDescription lVertexInputBindingDesc{};
	lVertexInputBindingDesc.binding	  = 0;
	lVertexInputBindingDesc.stride	  = sizeof(Vertex);
	lVertexInputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	eastl::array<VkVertexInputAttributeDescription, 1> lVertexAttrDescs{};

	// Position attribute
	lVertexAttrDescs[0].binding	 = 0;
	lVertexAttrDescs[0].location = 0;
	lVertexAttrDescs[0].format	 = VK_FORMAT_R32G32_SFLOAT; // Format the data will take (helps define size of data)
	lVertexAttrDescs[0].offset	 = offsetof(Vertex, Position);

	// Color attribute
	/*lVertexAttrDescs[1].binding	 = 0;
	lVertexAttrDescs[1].location = 1;
	lVertexAttrDescs[1].format	 = VK_FORMAT_R32G32B32_SFLOAT;
	lVertexAttrDescs[1].offset	 = offsetof(Vertex, Color);*/

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
	lRasterizer.frontFace				= VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

	eastl::array					 lDynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo lDynamicState{};
	lDynamicState.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	lDynamicState.dynamicStateCount = static_cast<uint32_t>(lDynamicStates.size());
	lDynamicState.pDynamicStates	= lDynamicStates.data();

	VkPipelineLayoutCreateInfo lPipelineLayoutInfo{};
	lPipelineLayoutInfo.sType		   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	lPipelineLayoutInfo.setLayoutCount = 1;
	lPipelineLayoutInfo.pSetLayouts	   = &mDescriptorSetLayout;
	// lPipelineLayoutInfo.pushConstantRangeCount = 1;
	// lPipelineLayoutInfo.pPushConstantRanges	   = &mPushConstantRange;

	RESULT_CONDITION_ENSURE(vkCreatePipelineLayout(mDevice, &lPipelineLayoutInfo, &mAllocationCallbacks,
												   &mGraphicsPipelineLayout) == VK_SUCCESS,
							VulkanFailedToCreateGraphicsPipelineLayout);

	VkGraphicsPipelineCreateInfo lPipelineInfo{};
	lPipelineInfo.sType				  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	lPipelineInfo.stageCount		  = static_cast<uint32_t>(lShaderStages.size());
	lPipelineInfo.pStages			  = lShaderStages.data();
	lPipelineInfo.pVertexInputState	  = &lVertexInputInfo;
	lPipelineInfo.pInputAssemblyState = &lInputAssembly;
	lPipelineInfo.pViewportState	  = &lViewportState;
	lPipelineInfo.pRasterizationState = &lRasterizer;
	lPipelineInfo.pMultisampleState	  = &lMultisampling;
	lPipelineInfo.pColorBlendState	  = &lColorBlending;
	lPipelineInfo.pDynamicState		  = &lDynamicState;
	lPipelineInfo.layout			  = mGraphicsPipelineLayout;
	lPipelineInfo.renderPass		  = mRenderPass;
	lPipelineInfo.subpass			  = 0;
	lPipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

	RESULT_CONDITION_ENSURE(vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &lPipelineInfo, &mAllocationCallbacks,
													  &mGraphicsPipeline) == VK_SUCCESS,
							VulkanFailedToCreateGraphicsPipeline);

	vkDestroyShaderModule(mDevice, lBasicVertShaderModule, &mAllocationCallbacks);
	vkDestroyShaderModule(mDevice, lBasicFragShaderModule, &mAllocationCallbacks);

	RESULT_OK();
}

INLINE void Manager::CreateFramebuffers(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	mSwapchain.Framebuffers.Resize(mSwapchain.ImageViews.Size());

	for (size_t i = 0; i < mSwapchain.Framebuffers.Size(); ++i)
	{
		const VkImageView lAttachments[] = {mSwapchain.ImageViews[i]};

		VkFramebufferCreateInfo lFramebufferInfo{};
		lFramebufferInfo.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		lFramebufferInfo.renderPass		 = mRenderPass;
		lFramebufferInfo.attachmentCount = 1;
		lFramebufferInfo.pAttachments	 = lAttachments;
		lFramebufferInfo.width			 = mSwapchain.Extent.width;
		lFramebufferInfo.height			 = mSwapchain.Extent.height;
		lFramebufferInfo.layers			 = 1;

		RESULT_CONDITION_ENSURE(vkCreateFramebuffer(mDevice, &lFramebufferInfo, &mAllocationCallbacks,
													&mSwapchain.Framebuffers[i]) == VK_SUCCESS,
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

	RESULT_CONDITION_ENSURE(vkCreateShaderModule(mDevice, &lCreateInfo, &mAllocationCallbacks, &lModule) == VK_SUCCESS,
							VulkanFailedToCreateShaderModule, {});

	RESULT_OK();
	return lModule;
}

INLINE void Manager::CreateUniformBuffers(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	mScreenPropertiesUniformBuffer.resize(mMaxFrames);
	mScreenPropertiesUniformBufferMemory.resize(mMaxFrames);

	Utils::CreateBufferCreateInfo lBufferInfo{};
	lBufferInfo.Device			 = mDevice;
	lBufferInfo.PhysicalDevice	 = mPhysicalDevice;
	lBufferInfo.BufferUsage		 = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	lBufferInfo.BufferProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	for (uint32_t lI = 0; lI < mMaxFrames; ++lI)
	{
		lBufferInfo.BufferSize = sizeof(ScreenProperties);
		RESULT_ENSURE_CALL(Utils::CreateBuffer(lBufferInfo, &mAllocationCallbacks, mScreenPropertiesUniformBuffer[lI],
											   mScreenPropertiesUniformBufferMemory[lI], RESULT_ARG_PASS));
	}

	RESULT_OK();
}

INLINE void Manager::CreateDescriptorPool(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	VkDescriptorPoolSize lScreenPropertiesPoolSize{};
	lScreenPropertiesPoolSize.type			  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	lScreenPropertiesPoolSize.descriptorCount = mMaxFrames;

	/*VkDescriptorPoolSize lModelPoolSize{};
	lModelPoolSize.type			   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	lModelPoolSize.descriptorCount = mMaxFrames;*/

	eastl::array lPoolSizes{lScreenPropertiesPoolSize};

	VkDescriptorPoolCreateInfo lPoolCreateInfo{};
	lPoolCreateInfo.sType		  = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	lPoolCreateInfo.maxSets		  = mMaxFrames;
	lPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(lPoolSizes.size());
	lPoolCreateInfo.pPoolSizes	  = lPoolSizes.data();

	RESULT_CONDITION_ENSURE(
		vkCreateDescriptorPool(mDevice, &lPoolCreateInfo, &mAllocationCallbacks, &mDescriptorPool) == VK_SUCCESS,
		VulkanFailedToCreateDescriptorPool);

	RESULT_OK();
}

INLINE void Manager::CreateDescriptorSets(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	mDescriptorSets.resize(mMaxFrames);

	const VkDescriptorSetLayout lSetLayouts[] = {mDescriptorSetLayout, mDescriptorSetLayout, mDescriptorSetLayout};

	VkDescriptorSetAllocateInfo lSetAllocInfo{};
	lSetAllocInfo.sType				 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	lSetAllocInfo.descriptorPool	 = mDescriptorPool;
	lSetAllocInfo.descriptorSetCount = mMaxFrames;
	lSetAllocInfo.pSetLayouts		 = lSetLayouts;

	RESULT_CONDITION_ENSURE(vkAllocateDescriptorSets(mDevice, &lSetAllocInfo, mDescriptorSets.data()) == VK_SUCCESS,
							VulkanFailedToAllocateDescriptorSets);

	for (uint32_t lI = 0; lI < mMaxFrames; ++lI)
	{
		/*VkDescriptorBufferInfo lViewProjectionBufferInfo{};
		lViewProjectionBufferInfo.buffer = mViewProjectionUniformBuffer[lI];
		lViewProjectionBufferInfo.offset = 0;
		lViewProjectionBufferInfo.range	 = sizeof(UboViewProjection);

		VkWriteDescriptorSet lMvpSetWrite{};
		lMvpSetWrite.sType			 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		lMvpSetWrite.dstSet			 = mDescriptorSets[lI];
		lMvpSetWrite.dstBinding		 = 0;
		lMvpSetWrite.dstArrayElement = 0;
		lMvpSetWrite.descriptorType	 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		lMvpSetWrite.descriptorCount = 1;
		lMvpSetWrite.pBufferInfo	 = &lViewProjectionBufferInfo;*/

		VkDescriptorBufferInfo lScreenPropertiesBufferInfo{};
		lScreenPropertiesBufferInfo.buffer = mScreenPropertiesUniformBuffer[lI];
		lScreenPropertiesBufferInfo.offset = 0;
		lScreenPropertiesBufferInfo.range  = sizeof(ScreenProperties);

		VkWriteDescriptorSet lScreenPropertiesSetWrite{};
		lScreenPropertiesSetWrite.sType			  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		lScreenPropertiesSetWrite.dstSet		  = mDescriptorSets[lI];
		lScreenPropertiesSetWrite.dstBinding	  = 0;
		lScreenPropertiesSetWrite.dstArrayElement = 0;
		lScreenPropertiesSetWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		lScreenPropertiesSetWrite.descriptorCount = 1;
		lScreenPropertiesSetWrite.pBufferInfo	  = &lScreenPropertiesBufferInfo;

		eastl::array lWriteDescriptors{lScreenPropertiesSetWrite};

		vkUpdateDescriptorSets(mDevice, static_cast<uint32_t>(lWriteDescriptors.size()), lWriteDescriptors.data(), 0,
							   nullptr);
	}

	RESULT_OK();
}

INLINE void Manager::UpdateUniformBuffers(const uint32_t ImageIndex, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	// Copy vp data
	void* lData{};
	vkMapMemory(mDevice, mScreenPropertiesUniformBufferMemory[ImageIndex], 0, sizeof(ScreenProperties), 0, &lData);
	memcpy(lData, &mScreenProperties, sizeof(ScreenProperties));
	vkUnmapMemory(mDevice, mScreenPropertiesUniformBufferMemory[ImageIndex]);

	RESULT_OK();
}

INLINE void Manager::CreateCommandPool(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	QueueFamilyIndices		lQueueFamilyIndices = FindQueueFamilies(mPhysicalDevice);
	VkCommandPoolCreateInfo lPoolInfo{};
	lPoolInfo.sType			   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	lPoolInfo.flags			   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	lPoolInfo.queueFamilyIndex = lQueueFamilyIndices.GraphicsFamily.value();

	RESULT_CONDITION_ENSURE(vkCreateCommandPool(mDevice, &lPoolInfo, &mAllocationCallbacks, &mCommandPool) ==
								VK_SUCCESS,
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

#if EDITOR
	if (mEditor)
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		Editor::Instance().RunExternal();
	}
#endif

	VkCommandBufferBeginInfo lBeginInfo{};
	lBeginInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	lBeginInfo.flags			= 0;	   // Optional
	lBeginInfo.pInheritanceInfo = nullptr; // Optional

	RESULT_CONDITION_ENSURE(vkBeginCommandBuffer(CommandBuffer, &lBeginInfo) == VK_SUCCESS,
							VulkanFailedToBeginCommandBuffer);

	VkRenderPassBeginInfo lRenderPassInfo{};
	lRenderPassInfo.sType			  = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	lRenderPassInfo.renderPass		  = mRenderPass;
	lRenderPassInfo.framebuffer		  = mSwapchain.Framebuffers[ImageIndex];
	lRenderPassInfo.renderArea.offset = {0, 0};
	lRenderPassInfo.renderArea.extent = mSwapchain.Extent;

	constexpr VkClearValue lClearColor = {{{0.f, 0.f, 0.f, 1.f}}};
	lRenderPassInfo.clearValueCount	   = 1;
	lRenderPassInfo.pClearValues	   = &lClearColor;

	vkCmdBeginRenderPass(CommandBuffer, &lRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

	const VkBuffer	   lVertexBuffer = mScreenMesh.GetVertexBuffer();
	const VkDeviceSize lOffset		 = 0;
	vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &lVertexBuffer, &lOffset);
	vkCmdBindIndexBuffer(CommandBuffer, mScreenMesh.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	// const auto lDynamicOffset = static_cast<uint32_t>(mModelUniformAlignment) * lI;

	vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipelineLayout, 0, 1,
		&mDescriptorSets[ImageIndex], 0, nullptr /*1, &lDynamicOffset*/);

	// vkCmdPushConstants(CommandBuffer, mGraphicsPipelineLayout, mPushConstantRange.stageFlags,
	// mPushConstantRange.offset, mPushConstantRange.size, &lMesh.GetModel().ModelMatrix);

	VkViewport lViewport{};
	lViewport.x		   = 0.0f;
	lViewport.y		   = 0.0f;
	lViewport.width	   = static_cast<float>(mSwapchain.Extent.width);
	lViewport.height   = static_cast<float>(mSwapchain.Extent.height);
	lViewport.minDepth = 0.0f;
	lViewport.maxDepth = 1.0f;
	vkCmdSetViewport(CommandBuffer, 0, 1, &lViewport);

	VkRect2D lScissor{};
	lScissor.offset = {0, 0};
	lScissor.extent = mSwapchain.Extent;
	vkCmdSetScissor(CommandBuffer, 0, 1, &lScissor);

	// vkCmdDraw(CommandBuffer, mMesh.GetVertexSize(), 1, 0, 0);
	vkCmdDrawIndexed(CommandBuffer, mScreenMesh.GetIndexSize(), 1, 0, 0, 0);

#if EDITOR
	if (mEditor)
	{
		ImGui::Render();
		ImDrawData* lMainDrawData = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(lMainDrawData, CommandBuffer);
	}
#endif

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
		RESULT_CONDITION_ENSURE(vkCreateSemaphore(mDevice, &lSemaphoreInfo, &mAllocationCallbacks,
												  &mImageAvailableSemaphores[i]) == VK_SUCCESS,
								VulkanFailedToCreateSemaphore);

		RESULT_CONDITION_ENSURE(vkCreateSemaphore(mDevice, &lSemaphoreInfo, &mAllocationCallbacks,
												  &mRenderFinishedSemaphores[i]) == VK_SUCCESS,
								VulkanFailedToCreateSemaphore);

		RESULT_CONDITION_ENSURE(vkCreateFence(mDevice, &lFenceInfo, &mAllocationCallbacks, &mInFlightFences[i]) ==
									VK_SUCCESS,
								VulkanFailedToCreateFence);
	}

	RESULT_OK();
}

INLINE void Manager::DestroySwapchain(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	for (const auto lSwapchainFramebuffer: mSwapchain.Framebuffers)
	{
		vkDestroyFramebuffer(mDevice, lSwapchainFramebuffer, &mAllocationCallbacks);
	}

	for (const auto lSwapchainImageView: mSwapchain.ImageViews)
	{
		vkDestroyImageView(mDevice, lSwapchainImageView, &mAllocationCallbacks);
	}

	vkDestroySwapchainKHR(mDevice, mSwapchain.Object, &mAllocationCallbacks);
	RESULT_OK();
}

INLINE void Manager::DestroySyncObjects(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	for (uint32_t lI = 0; lI < mMaxFrames; ++lI)
	{
		vkDestroySemaphore(mDevice, mImageAvailableSemaphores[lI], &mAllocationCallbacks);
		vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[lI], &mAllocationCallbacks);
		vkDestroyFence(mDevice, mInFlightFences[lI], &mAllocationCallbacks);
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

INLINE void Manager::AllocateDynamicBufferTransferSpace(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	// Get min alignment for mode. uniform object
	/*mScreenPropertiesUniformAlignment = Memory::Align(sizeof(ScreenProperties), mMinUniformBufferOffset);

	mScreenPropertiesTransferSpace = static_cast<ScreenProperties*>(default_allocator_t{}.allocate(
		mScreenPropertiesUniformAlignment, static_cast<int32_t>(mScreenPropertiesUniformAlignment)));*/

	RESULT_OK();
}

INLINE void Manager::UpdateModel(const uint32_t Id, const glm::mat4 Model, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	/*RESULT_CONDITION_ENSURE(Id < mMeshes.size(), InvalidIndex);
	mMeshes[Id].SetModel(Model);*/
	RESULT_OK();
}

INLINE bool Manager::IsDeviceSuitable(const VkPhysicalDevice Device) const
{
	const QueueFamilyIndices lIndices			  = FindQueueFamilies(Device);
	const bool				 lExtensionsSupported = CheckDeviceExtensionSupport(Device);

	bool lSwapChainAdequate = false;
	if (lExtensionsSupported)
	{
		const Swapchain::SupportDetails lSwapChainSupport = QuerySwapchainSupport(Device);
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
		vkGetPhysicalDeviceSurfaceSupportKHR(Device, lI, mSwapchain.Surface, &lPresentSupport);

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

	eastl::set<eastl::string, eastl::less<eastl::string>, default_allocator_t> lRequiredExtensions{
		mDeviceExtensions.begin(), mDeviceExtensions.end()};

	for (const auto& lExtension: lAvailableExtensions)
	{
		lRequiredExtensions.erase(lExtension.extensionName);
	}

	return lRequiredExtensions.empty();
}

INLINE Swapchain::SupportDetails Manager::QuerySwapchainSupport(const VkPhysicalDevice Device) const
{
	Swapchain::SupportDetails lDetails{default_allocator_t{}};

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, mSwapchain.Surface, &lDetails.Capabilities);

	uint32_t lFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(Device, mSwapchain.Surface, &lFormatCount, nullptr);

	if (lFormatCount != 0)
	{
		lDetails.Formats.Resize(lFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(Device, mSwapchain.Surface, &lFormatCount, lDetails.Formats.Data());
	}

	uint32_t lPresentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(Device, mSwapchain.Surface, &lPresentModeCount, nullptr);

	if (lPresentModeCount != 0)
	{
		lDetails.PresentModes.Resize(lPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(Device, mSwapchain.Surface, &lPresentModeCount,
												  lDetails.PresentModes.Data());
	}

	return lDetails;
}

INLINE VkSurfaceFormatKHR
Manager::ChooseSwapSurfaceFormat(const RawBuffer<VkSurfaceFormatKHR, default_allocator_t>& AvailableFormats)
{
	for (const auto& lAvailableFormat: AvailableFormats)
	{
		if (lAvailableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
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

	// return VK_PRESENT_MODE_IMMEDIATE_KHR;
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

} // namespace Render::Api

CLASS_VALIDATION(Render::Api::Mesh);
CLASS_VALIDATION(Render::Api::Manager);

#endif
