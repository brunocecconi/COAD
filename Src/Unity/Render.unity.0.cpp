
#include "Render/Shader.cpp"
#include "Render/Mesh.cpp"
#include "Render/Camera.cpp"
#include "Render/Texture.cpp"
#include "Render/ManagerRender.cpp"

#if PLATFORM_WINDOWS
#include "Render/Windows/RaytracingHelpers/BottomLevelASGenerator.cpp"
#include "Render/Windows/RaytracingHelpers/RaytracingPipelineGenerator.cpp"
#include "Render/Windows/RaytracingHelpers/RootSignatureGenerator.cpp"
#include "Render/Windows/RaytracingHelpers/ShaderBindingTableGenerator.cpp"
#include "Render/Windows/RaytracingHelpers/TopLevelASGenerator.cpp"
#endif
