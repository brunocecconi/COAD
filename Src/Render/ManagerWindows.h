
#ifdef RENDER_INCLUDE_PLATFORM_MANAGER

/** \file PlatformManager.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#include <vulkan/vulkan_win32.h>

#define VK_DEFINE_VAR(TYPE, STYPE)                                                                                     \
	Vk##TYPE l##TYPE{};                                                                                                \
	l##TYPE.sType = VK_STRUCTURE_TYPE_##STYPE

#define VK_DEFINE_CI_VAR(TYPE, STYPE)                                                                                  \
	Vk##TYPE##CreateInfo l##TYPE##Ci{};                                                                                \
	l##TYPE##Ci.sType = VK_STRUCTURE_TYPE_##STYPE##_CREATE_INFO

#define VK_DEFINE_CI_EXT_VAR(TYPE, STYPE) VK_DEFINE_CI_POSTFIX_VAR(TYPE, STYPE, EXT)

#define VK_DEFINE_CI_POSTFIX_VAR(TYPE, STYPE, POSTFIX)                                                                 \
	Vk##TYPE##CreateInfo##POSTFIX l##TYPE##Ci{};                                                                       \
	l##TYPE##Ci.sType = VK_STRUCTURE_TYPE_##STYPE##_CREATE_INFO##_##POSTFIX

/**
 * @brief Platform manager class.
 *
 * Designed to vulkan in windows.
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
	void Finalize(RESULT_PARAM_DEFINE);

private:
	void CreateInstance(RESULT_PARAM_DEFINE);
	void PickPhysicalDevice(RESULT_PARAM_DEFINE);
	bool SetSuitablePhysicalDevice(const eastl::vector_multimap<uint32_t, VkPhysicalDevice>::reverse_iterator& It,
								   const char** DeviceExtensionsData, uint32_t DeviceExtensionsSize);
	static bool CheckDeviceExtensionSupport(VkPhysicalDevice PhysicalDevice, const char** DeviceExtensionsData,
	                                        uint32_t DeviceExtensionsSize);
	static uint32_t RatePhysicalDeviceSuitability(VkPhysicalDevice PhysicalDevice);
	void			CreateLogicalDevice(RESULT_PARAM_DEFINE);
	void			CreateSurface(RESULT_PARAM_DEFINE);

private:
	enum class EQueueFamilyType
	{
		eGraphics,
		ePresent,
		eMax
	};

	struct QueueFamilyInfo
	{
		eastl::optional<uint32_t> Index[static_cast<int32_t>(EQueueFamilyType::eMax)];
		VkQueue					  Queue[static_cast<int32_t>(EQueueFamilyType::eMax)];

		NODISCARD bool IsComplete() const;
	};
	NODISCARD static QueueFamilyInfo FindQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);

private:
	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR	  SurfaceCapabilities;
		RawBuffer<VkSurfaceFormatKHR> Formats{DEBUG_NAME_VAL("PlatformRender")};
		RawBuffer<VkPresentModeKHR>	  PresentModes{DEBUG_NAME_VAL("PlatformRender")};
	};
	NODISCARD static SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice PhysicalDevice,
																   VkSurfaceKHR		Surface);
	NODISCARD static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const RawBuffer<VkSurfaceFormatKHR>& AvailableFormats);
	NODISCARD static VkPresentModeKHR	ChooseSwapPresentMode(const RawBuffer<VkPresentModeKHR>& AvailablePresentModes);
	NODISCARD static VkExtent2D			ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& SurfaceCapabilities,
														 glm::uvec2						 WindowSize);
	void								CreateSwapchain(RESULT_PARAM_DEFINE);
	void								CreateImageViews(RESULT_PARAM_DEFINE);

private:
	static eastl::unordered_set<eastl::string> GetSupportedExtensions();
	static eastl::unordered_set<eastl::string> GetSupportedLayers();

private:
	static void* AllocationCallaback(void* UserData, size_t Size, size_t Alignment,
									 VkSystemAllocationScope AllocationScope);
	static void* ReallocationCallback(void* UserData, void* Ptr, size_t Size, size_t Alignment,
									  VkSystemAllocationScope AllocationScope);
	static void	 FreeCallback(void* UserData, void* Ptr);
	static void	 AllocationInternalCallback(void* UserData, size_t Size, VkInternalAllocationType AllocationType,
											VkSystemAllocationScope AllocationScope);
	static void	 FreeInternalCallback(void* UserData, size_t Size, VkInternalAllocationType AllocationType,
									  VkSystemAllocationScope AllocationScope);
	VkAllocationCallbacks CreateAllocationCallbacks();

#if DEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT		MessageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT				MessageType,
														const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,
														void*										UserData);
#endif

private:
	friend class Manager;
	Engine::Window&		  mWindow;
	VkAllocationCallbacks mAllocator{};
	VkInstance			  mInstance{};
	VkPhysicalDevice	  mPhysicalDevice{};
	VkDevice			  mLogicalDevice{};
	QueueFamilyInfo		  mQueueFamilies{};
	VkSurfaceKHR		  mSurface{};
	VkSwapchainKHR		  mSwapchain{};
	RawBuffer<VkImage>	  mSwapchainImages{DEBUG_NAME_VAL("PlatformRender")};
	RawBuffer<VkImage>	  mSwapchainImageViews{DEBUG_NAME_VAL("PlatformRender")};
	VkFormat			  mSwapchainImageFormat{};
	VkExtent2D			  mSwapchainExtent{};

	const eastl::vector<const char*> mDeviceExtensions = {{VK_KHR_SWAPCHAIN_EXTENSION_NAME},
														  EASTLAllocatorType{DEBUG_NAME_VAL("PlatformRender")}};
#if DEBUG
	VkDebugUtilsMessengerEXT mDebugMessenger{};
#endif
};

INLINE PlatformManager::PlatformManager() : mWindow{Engine::Manager::Instance().mWindow}
{
}

INLINE PlatformManager::~PlatformManager()
{
}

INLINE void PlatformManager::Initialize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(CreateInstance(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateSurface(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(PickPhysicalDevice(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateLogicalDevice(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateSwapchain(RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE void PlatformManager::Finalize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(mLogicalDevice, eResultErrorNullPtr);
	RESULT_CONDITION_ENSURE(mInstance, eResultErrorNullPtr);

	// Destroy swap chain
	vkDestroySwapchainKHR(mLogicalDevice, mSwapchain, &mAllocator);
	mSwapchain = nullptr;

	// Destroy logical device
	vkDestroyDevice(mLogicalDevice, &mAllocator);
	mLogicalDevice = nullptr;

	// Destroy debug utils messenger
#if DEBUG
	if (const auto lFunc = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT")))
	{
		lFunc(mInstance, mDebugMessenger, &mAllocator);
		mDebugMessenger = nullptr;
	}
#endif

	// Destroy surface
	vkDestroySurfaceKHR(mInstance, mSurface, &mAllocator);
	mSurface = nullptr;

	// Destroy instance
	vkDestroyInstance(mInstance, &mAllocator);
	mInstance = nullptr;

	RESULT_OK();
}

INLINE void PlatformManager::CreateInstance(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	// Create application info
	VK_DEFINE_VAR(ApplicationInfo, APPLICATION_INFO);
	lApplicationInfo.pApplicationName	= "COAD";
	lApplicationInfo.applicationVersion = VK_API_VERSION_1_3;
	lApplicationInfo.pEngineName		= "No Engine";
	lApplicationInfo.engineVersion		= VK_API_VERSION_1_3;
	lApplicationInfo.apiVersion			= VK_API_VERSION_1_3;

	// Instance create info
	VK_DEFINE_CI_VAR(Instance, INSTANCE);
	lInstanceCi.pApplicationInfo = &lApplicationInfo;

	const auto& lSupportedExtensions = GetSupportedExtensions();
	const auto& lSupportedLayers	 = GetSupportedLayers();
	(void)lSupportedLayers;

	const char* lInstanceExtensions[] = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#if DEBUG
		,
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
	};

	// Required layers list
	const char* lRequiredLayers[] = {
		"VK_LAYER_LUNARG_screenshot"
#if DEBUG
		,
		"VK_LAYER_KHRONOS_validation"
#endif
	};

	// CHeck extensions
	for (const auto lReqExt: lInstanceExtensions)
	{
		RESULT_CONDITION_ENSURE(lSupportedExtensions.find_as(lReqExt) != lSupportedExtensions.cend(),
								eResultErrorPlatformRenderUnsupportedExtension);
	}

	// Set extensions
	lInstanceCi.enabledExtensionCount	= sizeof lInstanceExtensions / sizeof(char*);
	lInstanceCi.ppEnabledExtensionNames = lInstanceExtensions;

	// Check layers
	for (const auto lReqLayer: lRequiredLayers)
	{
		RESULT_CONDITION_ENSURE(lSupportedLayers.find_as(lReqLayer) != lSupportedLayers.cend(),
								eResultErrorPlatformRenderUnsupportedLayer);
	}

	// Set layers
	lInstanceCi.enabledLayerCount	= sizeof lRequiredLayers / sizeof(char*);
	lInstanceCi.ppEnabledLayerNames = lRequiredLayers;

	// Allocator callbacks
	mAllocator = CreateAllocationCallbacks();

	// Validation debug layer
#if DEBUG
	VK_DEFINE_CI_EXT_VAR(DebugUtilsMessenger, DEBUG_UTILS_MESSENGER);
	lDebugUtilsMessengerCi.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	lDebugUtilsMessengerCi.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
										 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
										 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	lDebugUtilsMessengerCi.pfnUserCallback = DebugCallback;
	if (const auto lFunc = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT")))
	{
		RESULT_CONDITION_ENSURE(lFunc(mInstance, &lDebugUtilsMessengerCi, &mAllocator, &mDebugMessenger) == VK_SUCCESS,
								eResultErrorPlatformRenderFailedToCreateDebugUtils);
	}
	lInstanceCi.pNext = &lDebugUtilsMessengerCi;
#endif

	// Create instance
	RESULT_CONDITION_ENSURE(vkCreateInstance(&lInstanceCi, &mAllocator, &mInstance) == VK_SUCCESS,
							eResultErrorPlatformRenderFailedToCreateInstance);

	RESULT_OK();
}

INLINE void PlatformManager::PickPhysicalDevice(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	// Get device count
	uint32_t lGpuCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &lGpuCount, nullptr);
	ENFORCE_MSG(lGpuCount > 0, "Not found a gpu.");

	// Get array of devices
	RawBuffer<VkPhysicalDevice> lGpus{lGpuCount, DEBUG_NAME_VAL("PlatformRender")};
	vkEnumeratePhysicalDevices(mInstance, &lGpuCount, lGpus.Data());

	// Use an ordered map to automatically sort candidates by increasing score
	eastl::vector_multimap<uint32_t, VkPhysicalDevice> lCandidates{DEBUG_NAME_VAL("PlatformRender")};
	lCandidates.reserve(lGpuCount);

	// Check if it is suitable
	for (const auto& lGpu: lGpus)
	{
		auto lScore = RatePhysicalDeviceSuitability(lGpu);
		lCandidates.emplace(lScore, lGpu);
	}

	RESULT_CONDITION_ENSURE(SetSuitablePhysicalDevice(lCandidates.rbegin(),
													  const_cast<const char**>(mDeviceExtensions.data()),
													  static_cast<uint32_t>(mDeviceExtensions.size())),
							eResultErrorPlatformRenderUnsuitableGpu);

	RESULT_OK();
}

INLINE bool PlatformManager::SetSuitablePhysicalDevice(
	const eastl::vector_multimap<uint32_t, VkPhysicalDevice>::reverse_iterator& It, const char** DeviceExtensionsData,
	const uint32_t DeviceExtensionsSize)
{
	// Fail with rate is zero.
	if (It->first == 0u)
	{
		return false;
	}

	// Check device extension support
	if (!CheckDeviceExtensionSupport(It->second, DeviceExtensionsData, DeviceExtensionsSize))
	{
		return false;
	}

	// Check swap chain support
	if (const SwapchainSupportDetails lSwapChainSupportDetails = QuerySwapchainSupport(It->second, mSurface);
		lSwapChainSupportDetails.Formats.IsEmpty() || lSwapChainSupportDetails.PresentModes.IsEmpty())
	{
		return false;
	}

	mPhysicalDevice = It->second;
	mQueueFamilies	= FindQueueFamilies(mPhysicalDevice, mSurface);
	return mPhysicalDevice != nullptr;
}

INLINE bool PlatformManager::CheckDeviceExtensionSupport(const VkPhysicalDevice PhysicalDevice,
												  const char**			 DeviceExtensionsData,
												  const uint32_t		 DeviceExtensionsSize)
{
	if (!DeviceExtensionsData || DeviceExtensionsSize == 0)
	{
		return false;
	}

	uint32_t lExtensionCount{};
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &lExtensionCount, nullptr);

	eastl::vector<VkExtensionProperties> lAvailableExtensions{lExtensionCount, DEBUG_NAME_VAL("PlatformRender")};
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &lExtensionCount, lAvailableExtensions.data());

	for (uint32_t i = 0; i < DeviceExtensionsSize; ++i)
	{
		const auto lExtension = DeviceExtensionsData[i];
		if (eastl::find_if(lAvailableExtensions.cbegin(), lAvailableExtensions.cend(),
						   [&lExtension](const VkExtensionProperties& Properties) {
							   return !strcmp(Properties.extensionName, lExtension);
						   }) == lAvailableExtensions.cend())
		{
			return false;
		}
	}

	return true;
}

INLINE uint32_t PlatformManager::RatePhysicalDeviceSuitability(const VkPhysicalDevice PhysicalDevice)
{
	VkPhysicalDeviceProperties lGpuProperties{};
	vkGetPhysicalDeviceProperties(PhysicalDevice, &lGpuProperties);

	VkPhysicalDeviceFeatures lGpuFeatures{};
	vkGetPhysicalDeviceFeatures(PhysicalDevice, &lGpuFeatures);

	uint32_t lScore{};

	// Check fail needed features
	if (!lGpuFeatures.geometryShader || !lGpuFeatures.textureCompressionBC)
	{
		return 0u;
	}

	// Discrete GPUs have a significant performance advantage
	if (lGpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		lScore += 3u;
	}
	else if (lGpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
	{
		lScore += 2u;
	}
	else if (lGpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
	{
		lScore += 1u;
	}

	// Maximum possible size of textures affects graphics quality
	lScore += lGpuProperties.limits.maxImageDimension2D;
	lScore += lGpuProperties.limits.maxMemoryAllocationCount;
	lScore += lGpuProperties.limits.maxSamplerAllocationCount;
	lScore += lGpuProperties.limits.maxViewports;

	const auto lGpuFeaturesBinaryData = reinterpret_cast<uint32_t*>(&lGpuFeatures);
	for (uint32_t lI = 0; lI < 55u; ++lI)
	{
		lScore += lGpuFeaturesBinaryData[lI];
	}

	return lScore;
}

INLINE void PlatformManager::CreateLogicalDevice(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	// Creates device queue create info
	constexpr float32_t					   lQueuePriority = 1.0f;
	constexpr auto						   lMaxQueues	  = static_cast<int32_t>(EQueueFamilyType::eMax);
	eastl::vector<VkDeviceQueueCreateInfo> lDeviceQueuesCi{DEBUG_NAME_VAL("PlatformRender")};
	eastl::unordered_set<uint32_t>		   lUniqueQueueFamilies{DEBUG_NAME_VAL("PlatformRender")};
	lUniqueQueueFamilies.insert(mQueueFamilies.Index[static_cast<int32_t>(EQueueFamilyType::eGraphics)].value());
	lUniqueQueueFamilies.insert(mQueueFamilies.Index[static_cast<int32_t>(EQueueFamilyType::ePresent)].value());
	VkPhysicalDeviceFeatures lDeviceFeatures{};

	lDeviceQueuesCi.reserve(lMaxQueues);
	for (auto& lQueueFamily: lUniqueQueueFamilies)
	{
		VK_DEFINE_CI_VAR(DeviceQueue, DEVICE_QUEUE);
		lDeviceQueueCi.queueFamilyIndex = lQueueFamily;
		lDeviceQueueCi.queueCount		= 1;
		lDeviceQueueCi.pQueuePriorities = &lQueuePriority;
		lDeviceQueuesCi.emplace_back(eastl::move(lDeviceQueueCi));
	}

	// Creates device create info
	VK_DEFINE_CI_VAR(Device, DEVICE);
	lDeviceCi.pQueueCreateInfos		  = lDeviceQueuesCi.data();
	lDeviceCi.queueCreateInfoCount	  = static_cast<uint32_t>(lDeviceQueuesCi.size());
	lDeviceCi.pEnabledFeatures		  = &lDeviceFeatures;
	lDeviceCi.enabledExtensionCount	  = static_cast<uint32_t>(mDeviceExtensions.size());
	lDeviceCi.ppEnabledExtensionNames = mDeviceExtensions.data();

	// Extensions and validation layers
	// TODO

	// Creates device
	RESULT_CONDITION_ENSURE(vkCreateDevice(mPhysicalDevice, &lDeviceCi, &mAllocator, &mLogicalDevice) == VK_SUCCESS,
							eResultErrorPlatformRenderFailedToCreateDevice);

	// Gets queues
	constexpr auto lGraphicsQueueIndex = static_cast<int32_t>(EQueueFamilyType::eGraphics);
	constexpr auto lPresentQueueIndex  = static_cast<int32_t>(EQueueFamilyType::ePresent);

	vkGetDeviceQueue(mLogicalDevice, mQueueFamilies.Index[lGraphicsQueueIndex].value(), 0,
					 &mQueueFamilies.Queue[lGraphicsQueueIndex]);
	vkGetDeviceQueue(mLogicalDevice, mQueueFamilies.Index[lPresentQueueIndex].value(), 0,
					 &mQueueFamilies.Queue[lPresentQueueIndex]);

	RESULT_CONDITION_ENSURE(mQueueFamilies.Queue[lGraphicsQueueIndex], eResultErrorPlatformRenderFailedToCreateDevice);
	RESULT_CONDITION_ENSURE(mQueueFamilies.Queue[lPresentQueueIndex], eResultErrorPlatformRenderFailedToCreateDevice);

	RESULT_OK();
}

INLINE void PlatformManager::CreateSurface(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	VK_DEFINE_CI_POSTFIX_VAR(Win32Surface, WIN32_SURFACE, KHR);
	lWin32SurfaceCi.hwnd	  = mWindow.GetHandle();
	lWin32SurfaceCi.hinstance = GetModuleHandle(nullptr);

	RESULT_CONDITION_ENSURE(vkCreateWin32SurfaceKHR(mInstance, &lWin32SurfaceCi, &mAllocator, &mSurface) == VK_SUCCESS,
							eResultErrorPlatformRenderFailedToCreateSurface);

	RESULT_OK();
}

INLINE bool PlatformManager::QueueFamilyInfo::IsComplete() const
{
	return Index[static_cast<int32_t>(EQueueFamilyType::eGraphics)].has_value() &&
		   Index[static_cast<int32_t>(EQueueFamilyType::ePresent)].has_value();
}

INLINE PlatformManager::QueueFamilyInfo PlatformManager::FindQueueFamilies(const VkPhysicalDevice PhysicalDevice,
																	const VkSurfaceKHR	   Surface)
{
	QueueFamilyInfo lResult{};

	if (!PhysicalDevice)
	{
		return lResult;
	}

	uint32_t lQueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &lQueueFamilyCount, nullptr);

	RawBuffer<VkQueueFamilyProperties> lQueueFamilies{lQueueFamilyCount, DEBUG_NAME_VAL("PlatformRender")};
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &lQueueFamilyCount, lQueueFamilies.Data());

	uint32_t lIndex{};
	VkBool32 lPresentSupport = false;
	for (const auto& lQueueFamily: lQueueFamilies)
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, lIndex, Surface, &lPresentSupport);

		if (lPresentSupport)
		{
			lResult.Index[static_cast<int32_t>(EQueueFamilyType::ePresent)] = lIndex;
		}
		if (lQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			lResult.Index[static_cast<int32_t>(EQueueFamilyType::eGraphics)] = lIndex;
		}
		if (lResult.IsComplete())
		{
			break;
		}
		++lIndex;
	}

	return lResult;
}

INLINE PlatformManager::SwapchainSupportDetails PlatformManager::QuerySwapchainSupport(const VkPhysicalDevice PhysicalDevice,
																				const VkSurfaceKHR	   Surface)
{
	SwapchainSupportDetails lDetails{};

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &lDetails.SurfaceCapabilities);

	uint32_t lFormatCount{};
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &lFormatCount, nullptr);

	if (lFormatCount)
	{
		lDetails.Formats.Resize(lFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &lFormatCount, lDetails.Formats.Data());
	}

	uint32_t lPresentModeCount{};
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &lPresentModeCount, nullptr);

	if (lPresentModeCount)
	{
		lDetails.PresentModes.Resize(lPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &lFormatCount, lDetails.PresentModes.Data());
	}

	return lDetails;
}

INLINE VkSurfaceFormatKHR PlatformManager::ChooseSwapSurfaceFormat(const RawBuffer<VkSurfaceFormatKHR>& AvailableFormats)
{
	for (const auto& lFormat: AvailableFormats)
	{
		if (lFormat.format == VK_FORMAT_B8G8R8A8_SRGB && lFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return lFormat;
		}
	}
	return AvailableFormats[0];
}

INLINE VkPresentModeKHR PlatformManager::ChooseSwapPresentMode(const RawBuffer<VkPresentModeKHR>& AvailablePresentModes)
{
	for (const auto& lPresentMode: AvailablePresentModes)
	{
		if (lPresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return VK_PRESENT_MODE_MAILBOX_KHR;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

INLINE VkExtent2D PlatformManager::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& SurfaceCapabilities,
											 const glm::uvec2				 WindowSize)
{
	if (SurfaceCapabilities.currentExtent.width != eastl::numeric_limits<uint32_t>::max())
	{
		return SurfaceCapabilities.currentExtent;
	}

	VkExtent2D lActualExtent = {WindowSize.x, WindowSize.y};
	lActualExtent.width		 = glm::clamp(lActualExtent.width, SurfaceCapabilities.minImageExtent.width,
										  SurfaceCapabilities.maxImageExtent.width);
	lActualExtent.height	 = glm::clamp(lActualExtent.height, SurfaceCapabilities.minImageExtent.height,
										  SurfaceCapabilities.maxImageExtent.height);

	return lActualExtent;
}

INLINE void PlatformManager::CreateSwapchain(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	const SwapchainSupportDetails lSwapChainSupport = QuerySwapchainSupport(mPhysicalDevice, mSurface);
	const VkSurfaceFormatKHR	  lSurfaceFormat	= ChooseSwapSurfaceFormat(lSwapChainSupport.Formats);
	mSwapchainImageFormat							= lSurfaceFormat.format;
	const VkPresentModeKHR lPresentMode				= ChooseSwapPresentMode(lSwapChainSupport.PresentModes);
	mSwapchainExtent = ChooseSwapExtent(lSwapChainSupport.SurfaceCapabilities, mWindow.GetSize());

	uint32_t lImageCount = lSwapChainSupport.SurfaceCapabilities.minImageCount + 1;
	if (lSwapChainSupport.SurfaceCapabilities.maxImageCount > 0 &&
		lImageCount > lSwapChainSupport.SurfaceCapabilities.maxImageCount)
	{
		lImageCount = lSwapChainSupport.SurfaceCapabilities.maxImageCount;
	}

	VK_DEFINE_CI_POSTFIX_VAR(Swapchain, SWAPCHAIN, KHR);
	lSwapchainCi.surface		  = mSurface;
	lSwapchainCi.minImageCount	  = lImageCount;
	lSwapchainCi.imageFormat	  = mSwapchainImageFormat;
	lSwapchainCi.imageColorSpace  = lSurfaceFormat.colorSpace;
	lSwapchainCi.imageExtent	  = mSwapchainExtent;
	lSwapchainCi.imageArrayLayers = 1;
	lSwapchainCi.imageUsage		  = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	lSwapchainCi.preTransform	  = lSwapChainSupport.SurfaceCapabilities.currentTransform;
	lSwapchainCi.compositeAlpha	  = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	lSwapchainCi.presentMode	  = lPresentMode;
	lSwapchainCi.clipped		  = VK_TRUE;
	lSwapchainCi.oldSwapchain	  = VK_NULL_HANDLE;

	RESULT_CONDITION_ENSURE(vkCreateSwapchainKHR(mLogicalDevice, &lSwapchainCi, &mAllocator, &mSwapchain) == VK_SUCCESS,
							eResultErrorPlatformRenderFailedToCreateSwapChain);

	vkGetSwapchainImagesKHR(mLogicalDevice, mSwapchain, &lImageCount, nullptr);
	mSwapchainImages.Resize(lImageCount);
	vkGetSwapchainImagesKHR(mLogicalDevice, mSwapchain, &lImageCount, mSwapchainImages.Data());

	RESULT_OK();
}

INLINE void PlatformManager::CreateImageViews(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	mSwapchainImageViews.Resize(mSwapchainImages.Size());

	for(const auto& lScImage : mSwapchainImages)
	{
		
	}

	RESULT_OK();
}

INLINE eastl::unordered_set<eastl::string> PlatformManager::GetSupportedExtensions()
{
	uint32_t lCount{};
	vkEnumerateInstanceExtensionProperties(nullptr, &lCount, nullptr);
	if (lCount == 0)
	{
		return eastl::unordered_set<eastl::string>{EASTLAllocatorType{DEBUG_NAME_VAL("PlatformRender")}};
	}

	eastl::vector<VkExtensionProperties> lExtensionProperties{lCount,
															  EASTLAllocatorType{DEBUG_NAME_VAL("PlatformRender")}};
	vkEnumerateInstanceExtensionProperties(nullptr, &lCount, lExtensionProperties.data());

	eastl::unordered_set<eastl::string> lSet{EASTLAllocatorType{DEBUG_NAME_VAL("PlatformRender")}};
	lSet.reserve(lCount);
	for (const auto& [extensionName, specVersion]: lExtensionProperties)
	{
		lSet.insert(eastl::string{extensionName, EASTLAllocatorType{DEBUG_NAME_VAL("PlatformRender")}});
	}

	return lSet;
}

INLINE eastl::unordered_set<eastl::string> PlatformManager::GetSupportedLayers()
{
	uint32_t lCount{};
	vkEnumerateInstanceLayerProperties(&lCount, nullptr);
	if (lCount == 0)
	{
		return eastl::unordered_set<eastl::string>{EASTLAllocatorType{DEBUG_NAME_VAL("PlatformRender")}};
	}

	eastl::vector<VkLayerProperties> lLayerProperties{lCount, EASTLAllocatorType{DEBUG_NAME_VAL("PlatformRender")}};
	vkEnumerateInstanceLayerProperties(&lCount, lLayerProperties.data());

	eastl::unordered_set<eastl::string> lSet{EASTLAllocatorType{DEBUG_NAME_VAL("PlatformRender")}};
	lSet.reserve(lCount);
	for (const auto& lLayer: lLayerProperties)
	{
		lSet.insert(eastl::string{lLayer.layerName, EASTLAllocatorType{DEBUG_NAME_VAL("PlatformRender")}});
	}
	return lSet;
}

INLINE void* PlatformManager::AllocationCallaback(void* UserData, const size_t Size, const size_t Alignment,
										   VkSystemAllocationScope AllocationScope)
{
	if (Size > 1024)
	{
		LOGC(Verbose, RenderPlatform, "Allocation[Alignment=%llu]: %f KB", Alignment,
			 static_cast<float32_t>(Size) / 1024.f);
	}

	return mi_malloc_aligned(Size, Alignment);
}

INLINE void* PlatformManager::ReallocationCallback(void* UserData, void* Ptr, const size_t Size, const size_t Alignment,
											VkSystemAllocationScope AllocationScope)
{
	if (Size > 1024)
	{
		LOGC(Verbose, RenderPlatform, "Reallocation[Alignment=%llu]: %f KB", Alignment,
			 static_cast<float32_t>(Size) / 1024.f);
	}
	return mi_realloc_aligned(Ptr, Size, Alignment);
}

INLINE void PlatformManager::FreeCallback(void* UserData, void* Ptr)
{
	LOGC(Verbose, RenderPlatform, "Free: %p", Ptr);
	mi_free(Ptr);
}

INLINE void PlatformManager::AllocationInternalCallback(void* UserData, size_t Size, VkInternalAllocationType AllocationType,
												 VkSystemAllocationScope AllocationScope)
{
	LOGC(Verbose, RenderPlatform, "Allocation[Alignment=%d]: %f KB", PLATFORM_ALIGNMENT,
		 static_cast<float32_t>(Size) / 1024.f);
}

INLINE void PlatformManager::FreeInternalCallback(void* UserData, size_t Size, VkInternalAllocationType AllocationType,
										   VkSystemAllocationScope AllocationScope)
{
	LOGC(Verbose, RenderPlatform, "Free: %f KB", static_cast<float32_t>(Size) / 1024.f);
}

INLINE VkAllocationCallbacks PlatformManager::CreateAllocationCallbacks()
{
	return VkAllocationCallbacks{this,		   AllocationCallaback,		   ReallocationCallback,
								 FreeCallback, AllocationInternalCallback, FreeInternalCallback};
}

#if DEBUG
INLINE VkBool32 PlatformManager::DebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
										VkDebugUtilsMessageTypeFlagsEXT				 MessageType,
										const VkDebugUtilsMessengerCallbackDataEXT* CallbackData, void* UserData)
{
	if ((MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) ==
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		LOGC(Verbose, RenderPlatform, "\nMessage ID: %s - Message: %s", CallbackData->pMessageIdName,
			 CallbackData->pMessage);
	}
	else if ((MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) ==
			 VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		LOGC(Info, RenderPlatform, "\nMessage ID: %s - Message: %s", CallbackData->pMessageIdName,
			 CallbackData->pMessage);
	}
	else if ((MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) ==
			 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		LOGC(Warning, RenderPlatform, "\nMessage ID: %s - Message: %s", CallbackData->pMessageIdName,
			 CallbackData->pMessage);
	}
	else if ((MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) ==
			 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		LOGC(Error, RenderPlatform, "\nMessage ID: %s - Message: %s", CallbackData->pMessageIdName,
			 CallbackData->pMessage);
	}
	return VK_FALSE;
}
#endif

#endif
