
namespace Render::Api
{

using shader_compile_data_t = RawBuffer<char, default_allocator_t>;

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

} // namespace Render::Api
