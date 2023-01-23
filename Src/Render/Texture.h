
#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include "Render/Common.h"

#include <EASTL/span.h>

namespace Render
{

#if PLATFORM_WINDOWS
#if D3D12_ENABLED
#elif VULKAN_ENABLED
using texture_handle_t = VkImage;
#endif
#endif

/**
 * @brief Render texture class.
 *
 * Store memory in gpu and it has just handles to memory.
 *
 */
class Texture
{
	CLASS_BODY(Texture);

public:
	/**
	 * @brief Texture type.
	 *
	 */
	enum EType
	{
		/**
		 * @brief Texture 2d.
		 *
		 * Default value.
		 *
		 */
		e2D,
		e3D,
		eCubeMap,

		/**
		 * @brief Used to render the scene from a perspective.
		 *
		 */
		eRenderTarget
	};

public:
	EXPLICIT Texture(EType Type, eastl::span<uint8_t> Data, RESULT_PARAM_DEFINE);
	~Texture();

public:
	NODISCARD void SetData(eastl::span<uint8_t> NewData, RESULT_PARAM_DEFINE) const;
	NODISCARD eastl::span<uint8_t> GetData() const;
	NODISCARD texture_handle_t	   GetHandle() const;

private:
	texture_handle_t mHandle{};
	EType			 mType{};
};

} // namespace Render

CLASS_VALIDATION(Render::Texture);

#endif
