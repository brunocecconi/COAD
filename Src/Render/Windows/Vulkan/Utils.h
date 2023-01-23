
namespace Render::Api
{

class Utils
{
public:
	struct CreateBufferCreateInfo
	{
		VkPhysicalDevice	  PhysicalDevice{};
		VkDevice			  Device{};
		VkDeviceSize		  BufferSize{};
		VkBufferUsageFlags	  BufferUsage{};
		VkMemoryPropertyFlags BufferProperties =
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	};

	struct CopyBufferCreateInfo
	{
		VkDevice	  Device{};
		VkQueue		  TransferQueue{};
		VkCommandPool TransferCommandPool{};
		VkBuffer	  SrcBuffer{};
		VkBuffer	  DstBuffer{};
		VkDeviceSize  BufferSize{};
	};

	static void CreateBuffer(CreateBufferCreateInfo& Info, const VkAllocationCallbacks* Allocator, VkBuffer& Buffer,
							 VkDeviceMemory& BufferMemory, RESULT_PARAM_DEFINE);
	static void CreateVertexBuffer(CreateBufferCreateInfo& Info, const VkAllocationCallbacks* Allocator,
								   VkBuffer& Buffer, VkDeviceMemory& BufferMemory, RESULT_PARAM_DEFINE);
	static void CreateStagingBuffer(CreateBufferCreateInfo& Info, const VkAllocationCallbacks* Allocator,
									VkBuffer& Buffer, VkDeviceMemory& BufferMemory, RESULT_PARAM_DEFINE);
	static void DestroyBuffer(VkDevice Device, VkBuffer& Buffer, VkDeviceMemory& BufferMemory,
							  const VkAllocationCallbacks* Allocator, RESULT_PARAM_DEFINE);
	static void CopyBuffer(CopyBufferCreateInfo& Info, RESULT_PARAM_DEFINE);
	NODISCARD static uint32_t FindBufferMemoryTypeIndex(VkPhysicalDevice PhysicalDevice, uint32_t AllowedTypes,
														VkMemoryPropertyFlags PropertyFlags, RESULT_PARAM_DEFINE);
};

INLINE void Utils::CreateBuffer(CreateBufferCreateInfo& Info, const VkAllocationCallbacks* Allocator, VkBuffer& Buffer,
								VkDeviceMemory& BufferMemory, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(Info.PhysicalDevice && Info.Device, NullPtr);
	RESULT_CONDITION_ENSURE(Info.BufferSize, ZeroSize);
	RESULT_CONDITION_ENSURE(!Buffer, PtrIsNotNull);
	RESULT_CONDITION_ENSURE(!BufferMemory, PtrIsNotNull);

	VkBufferCreateInfo lBufferCreateInfo{};
	lBufferCreateInfo.sType		  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	lBufferCreateInfo.size		  = Info.BufferSize;
	lBufferCreateInfo.usage		  = Info.BufferUsage;
	lBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	RESULT_CONDITION_ENSURE(vkCreateBuffer(Info.Device, &lBufferCreateInfo, Allocator, &Buffer) == VK_SUCCESS,
							VulkanFailedToCreateBuffer);

	// Get buffer memory requirements
	VkMemoryRequirements lMemoryRequirements{};
	vkGetBufferMemoryRequirements(Info.Device, Buffer, &lMemoryRequirements);

	// Allocate memory to buffer
	VkMemoryAllocateInfo lMemoryAllocateInfo{};
	lMemoryAllocateInfo.sType		   = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	lMemoryAllocateInfo.allocationSize = lMemoryRequirements.size;

	// Find memory type index
	RESULT_ENSURE_CALL(lMemoryAllocateInfo.memoryTypeIndex = FindBufferMemoryTypeIndex(
						   Info.PhysicalDevice, lMemoryRequirements.memoryTypeBits,
						   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						   RESULT_ARG_PASS));

	RESULT_CONDITION_ENSURE(vkAllocateMemory(Info.Device, &lMemoryAllocateInfo, Allocator, &BufferMemory) == VK_SUCCESS,
							VulkanFailedToAllocateMemory);

	// Bind allocated memory to buffer
	vkBindBufferMemory(Info.Device, Buffer, BufferMemory, 0);

	RESULT_OK();
}

INLINE void Utils::CreateVertexBuffer(CreateBufferCreateInfo& Info, const VkAllocationCallbacks* Allocator,
									  VkBuffer& Buffer, VkDeviceMemory& BufferMemory, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(Info.BufferUsage == 0, VulkanUnexpectedBufferUsage);
	Info.BufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	RESULT_ENSURE_CALL(CreateBuffer(Info, Allocator, Buffer, BufferMemory, RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE void Utils::CreateStagingBuffer(CreateBufferCreateInfo& Info, const VkAllocationCallbacks* Allocator,
									   VkBuffer& Buffer, VkDeviceMemory& BufferMemory, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(Info.BufferUsage == 0, VulkanUnexpectedBufferUsage);
	Info.BufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	RESULT_ENSURE_CALL(CreateBuffer(Info, Allocator, Buffer, BufferMemory, RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE void Utils::DestroyBuffer(const VkDevice Device, VkBuffer& Buffer, VkDeviceMemory& BufferMemory,
								 const VkAllocationCallbacks* Allocator, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(Buffer && BufferMemory, NullPtr);
	vkDestroyBuffer(Device, Buffer, Allocator);
	vkFreeMemory(Device, BufferMemory, Allocator);
	Buffer		 = nullptr;
	BufferMemory = nullptr;
	RESULT_OK();
}

INLINE void Utils::CopyBuffer(CopyBufferCreateInfo& Info, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(
		Info.Device && Info.TransferQueue && Info.TransferCommandPool && Info.SrcBuffer && Info.DstBuffer, NullPtr);
	RESULT_CONDITION_ENSURE(Info.BufferSize, ZeroSize);

	VkCommandBuffer lTransferCommandBuffer{};

	VkCommandBufferAllocateInfo lAllocInfo{};
	lAllocInfo.sType			  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	lAllocInfo.level			  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	lAllocInfo.commandPool		  = Info.TransferCommandPool;
	lAllocInfo.commandBufferCount = 1;

	RESULT_CONDITION_ENSURE(vkAllocateCommandBuffers(Info.Device, &lAllocInfo, &lTransferCommandBuffer) == VK_SUCCESS,
							VulkanFailedToCreateCommandBuffer);

	VkCommandBufferBeginInfo lBeginInfo{};
	lBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	lBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	RESULT_CONDITION_ENSURE(vkBeginCommandBuffer(lTransferCommandBuffer, &lBeginInfo) == VK_SUCCESS,
							VulkanFailedToBeginCommandBuffer);
	{
		VkBufferCopy lBufferCopyInfo{};
		lBufferCopyInfo.srcOffset = 0;
		lBufferCopyInfo.dstOffset = 0;
		lBufferCopyInfo.size	  = Info.BufferSize;

		vkCmdCopyBuffer(lTransferCommandBuffer, Info.SrcBuffer, Info.DstBuffer, 1, &lBufferCopyInfo);
	}
	RESULT_CONDITION_ENSURE(vkEndCommandBuffer(lTransferCommandBuffer) == VK_SUCCESS, VulkanFailedToEndCommandBuffer);

	VkSubmitInfo lSubmitInfo{};
	lSubmitInfo.sType			   = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	lSubmitInfo.commandBufferCount = 1;
	lSubmitInfo.pCommandBuffers	   = &lTransferCommandBuffer;
	RESULT_CONDITION_ENSURE(vkQueueSubmit(Info.TransferQueue, 1, &lSubmitInfo, nullptr) == VK_SUCCESS,
							VulkanFailedToSubmitTransferQueue);
	RESULT_CONDITION_ENSURE(vkQueueWaitIdle(Info.TransferQueue) == VK_SUCCESS, VulkanFailedToWaitQueueIdle);

	vkFreeCommandBuffers(Info.Device, Info.TransferCommandPool, 1, &lTransferCommandBuffer);

	RESULT_OK();
}

INLINE uint32_t Utils::FindBufferMemoryTypeIndex(const VkPhysicalDevice PhysicalDevice, const uint32_t AllowedTypes,
												 const VkMemoryPropertyFlags PropertyFlags, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST({});
	RESULT_CONDITION_ENSURE(AllowedTypes && PropertyFlags, ZeroSize, {});

	VkPhysicalDeviceMemoryProperties lMemoryProperties{};
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &lMemoryProperties);

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
