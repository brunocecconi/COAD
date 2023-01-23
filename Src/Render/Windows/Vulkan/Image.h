
namespace Render::Api
{

class Image
{
	CLASS_BODY_NON_COPYABLE_OMIT_MOVE(Image);

public:
	struct Info
	{
		Manager*			  Manager;
		VkPhysicalDevice	  PhysicalDevice;
		VkDevice			  Device;
		VkQueue				  TransferQueue;
		VkCommandPool		  TransferCmdPool;
		uint32_t			  Width, Height;
		eastl::span<uint32_t> Data;
	};

	Image() = default;
	EXPLICIT Image(Info& Info, RESULT_PARAM_DEFINE);
	~Image();

public:
	void CreateBuffer(Info& Info, RESULT_PARAM_DEFINE);
	void DestroyBuffer(RESULT_PARAM_DEFINE);

private:
	Manager*		 mManager{};
	VkImage			 mImage{};
	VkImageView		 mImageView{};
	VkDeviceMemory	 mMemory{};
	VkPhysicalDevice mPhysicalDevice{};
	VkDevice		 mDevice{};
	VkQueue			 mTransferQueue{};
	VkCommandPool	 mTransferCmdPool{};
};


INLINE Image::Image(Info& Info, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	constexpr VkFormat lVulkanFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

	// Create image
	{
		VkImageCreateInfo info = {};
		info.sType			   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.imageType		   = VK_IMAGE_TYPE_2D;
		info.format			   = lVulkanFormat;
		info.extent.width	   = Info.Width;
		info.extent.height	   = Info.Height;
		info.extent.depth	   = 1;
		info.mipLevels		   = 1;
		info.arrayLayers	   = 1;
		info.samples		   = VK_SAMPLE_COUNT_1_BIT;
		info.tiling			   = VK_IMAGE_TILING_OPTIMAL;
		info.usage			   = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		info.sharingMode	   = VK_SHARING_MODE_EXCLUSIVE;
		info.initialLayout	   = VK_IMAGE_LAYOUT_UNDEFINED;
		vkCreateImage(Info.Device, &info, nullptr, &mImage);

		VkMemoryRequirements lReq;
		vkGetImageMemoryRequirements(mDevice, mImage, &lReq);
		VkMemoryAllocateInfo lAllocInfo = {};
		lAllocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		lAllocInfo.allocationSize		= lReq.size;
		lAllocInfo.memoryTypeIndex		= Utils::FindBufferMemoryTypeIndex(
			 Info.PhysicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, lReq.memoryTypeBits);
		vkAllocateMemory(mDevice, &lAllocInfo, nullptr, &mMemory);
		vkBindImageMemory(mDevice, mImage, mMemory, 0);
	}

	// Create image view
	{
		VkImageViewCreateInfo info		 = {};
		info.sType						 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.image						 = mImage;
		info.viewType					 = VK_IMAGE_VIEW_TYPE_2D;
		info.format						 = lVulkanFormat;
		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.layerCount = 1;
		vkCreateImageView(mDevice, &info, nullptr, &mImageView);
	}

	RESULT_OK();
}

INLINE void Image::CreateBuffer(Info& Info, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	/*VkBuffer lStagingBuffer{};
	VkDeviceMemory lStagingBufferMemory{};

	Utils::CreateBufferCreateInfo lStagingBufferCreateInfo{};
	lStagingBufferCreateInfo.BufferSize = Info.Width * Info.Height * 4u;
	lStagingBufferCreateInfo.Device = Info.Device;
	lStagingBufferCreateInfo.PhysicalDevice = Info.PhysicalDevice;
	lStagingBufferCreateInfo.BufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;*/

	// RESULT_ENSURE_CALL(Utils::CreateBuffer(RESULT_ARG_PASS));

	RESULT_OK();
}

INLINE void Image::DestroyBuffer(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_OK();
}

}