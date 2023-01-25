
namespace Render::Api
{

struct ScreenProperties
{
	glm::vec2 Resolution;
};

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
	struct Info
	{
		VkPhysicalDevice							 PhysicalDevice{};
		VkDevice									 Device{};
		VkQueue										 TransferQueue{};
		VkCommandPool								 TransferCmdPool{};
		VkAllocationCallbacks*						 AllocatorCallbacks{};
		eastl::vector<Vertex, default_allocator_t>	 Vertices{};
		eastl::vector<uint32_t, default_allocator_t> Indices{};
	};

	Mesh() = default;
	EXPLICIT Mesh(const Info& Info, RESULT_PARAM_DEFINE);
	~Mesh();

public:
	Mesh(Mesh&& Other) NOEXCEPT;
	Mesh& operator=(Mesh&& Other) NOEXCEPT;

public:
	NODISCARD static Mesh Square(const Info& Info, glm::vec2 Location, RESULT_PARAM_DEFINE);

public:
	void					  DestroyBuffers(RESULT_PARAM_DEFINE);
	NODISCARD uint32_t		  GetVertexSize() const;
	NODISCARD VkBuffer		  GetVertexBuffer() const;
	NODISCARD uint32_t		  GetIndexSize() const;
	NODISCARD VkBuffer		  GetIndexBuffer() const;

private:
	void CreateVertexBuffer(const eastl::vector<Vertex, default_allocator_t>& Vertices, RESULT_PARAM_DEFINE);
	void CreateIndexBuffer(const eastl::vector<uint32_t, default_allocator_t>& Indices, RESULT_PARAM_DEFINE);

private:
	uint32_t	   mVertexSize{};
	VkBuffer	   mVertexBuffer{};
	VkDeviceMemory mVertexBufferMemory{};

	uint32_t	   mIndexSize{};
	VkBuffer	   mIndexBuffer{};
	VkDeviceMemory mIndexBufferMemory{};

	Info mInfo{};
};

INLINE Mesh::Mesh(const Info& Info, RESULT_PARAM_IMPL)
	: mVertexSize{static_cast<uint32_t>(Info.Vertices.size())},
	  mIndexSize{static_cast<uint32_t>(Info.Indices.size())}, mInfo{Info}
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(CreateVertexBuffer(Info.Vertices, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(CreateIndexBuffer(Info.Indices, RESULT_ARG_PASS));
	RESULT_OK();
}

INLINE Mesh::~Mesh()
{
	if (mVertexBuffer && mVertexBufferMemory && mIndexBuffer && mIndexBufferMemory)
	{
		DestroyBuffers();
	}
}

INLINE Mesh::Mesh(Mesh&& Other) NOEXCEPT: mVertexSize{Other.mVertexSize},
										  mVertexBuffer{Other.mVertexBuffer},
										  mVertexBufferMemory{Other.mVertexBufferMemory},
										  mIndexSize{Other.mIndexSize},
										  mIndexBuffer{Other.mIndexBuffer},
										  mIndexBufferMemory{Other.mIndexBufferMemory},
										  mInfo{eastl::move(Other.mInfo)}
{
	Other.mInfo				  = Info{};
	Other.mVertexSize		  = 0;
	Other.mVertexBuffer		  = nullptr;
	Other.mVertexBufferMemory = nullptr;
	Other.mIndexSize		  = 0;
	Other.mIndexBuffer		  = nullptr;
	Other.mIndexBufferMemory  = nullptr;
}

INLINE Mesh& Mesh::operator=(Mesh&& Other) NOEXCEPT
{
	mInfo		= eastl::move(Other.mInfo);
	Other.mInfo = Info{};

	mVertexSize				  = Other.mVertexSize;
	mVertexBuffer			  = Other.mVertexBuffer;
	mVertexBufferMemory		  = Other.mVertexBufferMemory;
	mIndexSize				  = Other.mIndexSize;
	mIndexBuffer			  = Other.mIndexBuffer;
	mIndexBufferMemory		  = Other.mIndexBufferMemory;
	Other.mVertexSize		  = 0;
	Other.mVertexBuffer		  = nullptr;
	Other.mVertexBufferMemory = nullptr;
	Other.mIndexSize		  = 0;
	Other.mIndexBuffer		  = nullptr;
	Other.mIndexBufferMemory  = nullptr;

	return *this;
}

INLINE Mesh Mesh::Square(const Info& Info, glm::vec2 Location, RESULT_PARAM_IMPL)
{
	Mesh lMesh{};
	auto lInfo	   = Info;
	lInfo.Vertices = {{{Location.x + 0.5f, Location.y + -0.5f}},
					  {{Location.x + 0.5f, Location.y + 0.5f}},
					  {{Location.x + -0.5f, Location.y + 0.5f}},
					  {{Location.x + -0.5f, Location.y + -0.5f}}}; // 0,1,2,2,3,0

	lInfo.Indices = {{2, 1, 0, 0, 3, 2}};
	RESULT_ENSURE_CALL(lMesh = Mesh(lInfo, RESULT_ARG_PASS), lMesh);
	return lMesh;
}

INLINE void Mesh::DestroyBuffers(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(Utils::DestroyBuffer(mInfo.Device, mVertexBuffer, mVertexBufferMemory, mInfo.AllocatorCallbacks,
											RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(Utils::DestroyBuffer(mInfo.Device, mIndexBuffer, mIndexBufferMemory, mInfo.AllocatorCallbacks,
											RESULT_ARG_PASS));
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

INLINE uint32_t Mesh::GetIndexSize() const
{
	return mIndexSize;
}

INLINE VkBuffer Mesh::GetIndexBuffer() const
{
	return mIndexBuffer;
}

INLINE void Mesh::CreateVertexBuffer(const eastl::vector<Vertex, default_allocator_t>& Vertices, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	Utils::CreateBufferCreateInfo lBufferInfo{};
	lBufferInfo.PhysicalDevice = mInfo.PhysicalDevice;
	lBufferInfo.Device		   = mInfo.Device;
	lBufferInfo.BufferSize	   = sizeof(Vertex) * Vertices.size();

	// Temporary buffer to "stage" vertex data before transferring to GPU
	VkBuffer	   lStagingBuffer{};
	VkDeviceMemory lStagingBufferMemory{};
	lBufferInfo.BufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	RESULT_ENSURE_CALL(Utils::CreateBuffer(lBufferInfo, mInfo.AllocatorCallbacks, lStagingBuffer, lStagingBufferMemory,
										   RESULT_ARG_PASS));

	// Map memory to vertex buffer
	void* lData{}; // 1. Create pointer to RAM
	vkMapMemory(lBufferInfo.Device, lStagingBufferMemory, 0, lBufferInfo.BufferSize, 0,
				&lData);									 // 2. "Map" the device memory to raw pointer in ram.
	memcpy(lData, Vertices.data(), lBufferInfo.BufferSize);	 // 3. Copy memory from vertices to mapped data pointer.
	vkUnmapMemory(lBufferInfo.Device, lStagingBufferMemory); // 4. Unmap device memory.

	// Create buffer with transfer dst bit to mark as recipient of transfer data
	lBufferInfo.BufferUsage		 = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	lBufferInfo.BufferProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	RESULT_ENSURE_CALL(Utils::CreateBuffer(lBufferInfo, mInfo.AllocatorCallbacks, mVertexBuffer, mVertexBufferMemory,
										   RESULT_ARG_PASS));

	Utils::CopyBufferCreateInfo lCopyBufferCi{};
	lCopyBufferCi.Device			  = mInfo.Device;
	lCopyBufferCi.TransferQueue		  = mInfo.TransferQueue;
	lCopyBufferCi.TransferCommandPool = mInfo.TransferCmdPool;
	lCopyBufferCi.BufferSize		  = lBufferInfo.BufferSize;
	lCopyBufferCi.SrcBuffer			  = lStagingBuffer;
	lCopyBufferCi.DstBuffer			  = mVertexBuffer;

	RESULT_ENSURE_CALL(Utils::CopyBuffer(lCopyBufferCi, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(Utils::DestroyBuffer(lCopyBufferCi.Device, lStagingBuffer, lStagingBufferMemory,
											mInfo.AllocatorCallbacks, RESULT_ARG_PASS));

	RESULT_OK();
}

INLINE void Mesh::CreateIndexBuffer(const eastl::vector<uint32_t, default_allocator_t>& Indices, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	Utils::CreateBufferCreateInfo lBufferInfo{};
	lBufferInfo.PhysicalDevice = mInfo.PhysicalDevice;
	lBufferInfo.Device		   = mInfo.Device;
	lBufferInfo.BufferSize	   = sizeof(uint32_t) * Indices.size();

	VkBuffer	   lStagingBuffer{};
	VkDeviceMemory lStagingBufferMemory{};
	lBufferInfo.BufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	RESULT_ENSURE_CALL(Utils::CreateBuffer(lBufferInfo, mInfo.AllocatorCallbacks, lStagingBuffer, lStagingBufferMemory,
										   RESULT_ARG_PASS));

	void* lData{};
	vkMapMemory(mInfo.Device, lStagingBufferMemory, 0, lBufferInfo.BufferSize, 0, &lData);
	memcpy(lData, Indices.data(), lBufferInfo.BufferSize);
	vkUnmapMemory(mInfo.Device, lStagingBufferMemory);

	lBufferInfo.BufferUsage		 = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	lBufferInfo.BufferProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	RESULT_ENSURE_CALL(
		Utils::CreateBuffer(lBufferInfo, mInfo.AllocatorCallbacks, mIndexBuffer, mIndexBufferMemory, RESULT_ARG_PASS));

	Utils::CopyBufferCreateInfo lCopyBufferCi{};
	lCopyBufferCi.Device			  = mInfo.Device;
	lCopyBufferCi.TransferQueue		  = mInfo.TransferQueue;
	lCopyBufferCi.TransferCommandPool = mInfo.TransferCmdPool;
	lCopyBufferCi.BufferSize		  = lBufferInfo.BufferSize;
	lCopyBufferCi.SrcBuffer			  = lStagingBuffer;
	lCopyBufferCi.DstBuffer			  = mIndexBuffer;
	RESULT_ENSURE_CALL(Utils::CopyBuffer(lCopyBufferCi, RESULT_ARG_PASS));

	RESULT_ENSURE_CALL(Utils::DestroyBuffer(mInfo.Device, lStagingBuffer, lStagingBufferMemory,
											mInfo.AllocatorCallbacks, RESULT_ARG_PASS));

	RESULT_OK();
}

} // namespace Render::Api
