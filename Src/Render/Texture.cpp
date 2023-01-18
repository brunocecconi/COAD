
#include "Render/Texture.h"
#include "Render/Manager.h"

namespace Render
{

Texture::Texture(const EType Type, eastl::span<uint8_t> Data, RESULT_PARAM_IMPL) : mType{Type}
{
	RESULT_ENSURE_LAST();
	UNUSED(Data);
	switch (Type)
	{
	case e2D: {
		break;
	}
	case e3D: {
		break;
	}
	case eCubeMap: {
		break;
	}
	case eRenderTarget: {
		break;
	}
	default:;
	}
	RESULT_OK();
}

Texture::~Texture()
{
}

void Texture::SetData(const eastl::span<uint8_t> NewData, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(!NewData.empty(), ZeroSize);
	// RESULT_ENSURE_CALL(Instance().SetTextureData(mHandle.Get(), NewData, RESULT_ARG_PASS));
	RESULT_OK();
}

eastl::span<uint8_t> Texture::GetData() const
{
	if (!mHandle)
	{
		return {};
	}

	return {};
}

texture_handle_t Texture::GetHandle() const
{
	return nullptr;
}

} // namespace Render
