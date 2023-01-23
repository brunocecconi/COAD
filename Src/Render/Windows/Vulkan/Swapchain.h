
namespace Render::Api
{

struct Swapchain
{
	EXPLICIT Swapchain(const default_allocator_t& Allocator)
		: Images{Allocator}, ImageViews{Allocator}, Framebuffers{Allocator}
	{
	}

	VkSwapchainKHR								  Object{};
	RawBuffer<VkImage, default_allocator_t>		  Images{};
	VkFormat									  ImageFormat{};
	VkExtent2D									  Extent{};
	RawBuffer<VkImageView, default_allocator_t>	  ImageViews{};
	RawBuffer<VkFramebuffer, default_allocator_t> Framebuffers{};

	struct SupportDetails
	{
		EXPLICIT SupportDetails(const default_allocator_t& Allocator) : Formats{Allocator}, PresentModes{Allocator}
		{
		}

		VkSurfaceCapabilitiesKHR						   Capabilities{};
		RawBuffer<VkSurfaceFormatKHR, default_allocator_t> Formats{};
		RawBuffer<VkPresentModeKHR, default_allocator_t>   PresentModes{};
	};
};

}
