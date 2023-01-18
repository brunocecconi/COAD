
function EditorEnable()
defines { "EDITOR=1" }
filter { "configurations:Debug or configurations:Development or configurations:Profile", "platforms:Windows" }
	links { "imguid" }
	
filter { "configurations:Release", "platforms:Windows" }
	links { "imgui" }
end

function VulkanEnable(ProjectName)

if ProjectName == "COAD" then
filter { "platforms:Windows or platforms:Linux" }
	defines { "VULKAN_ENABLED=1" }

elseif ProjectName == "COAD-Cmd" then
filter { "platforms:Windows or platforms:Linux" }
	defines { "VULKAN_ENABLED=1" }

filter { "configurations:Debug or configurations:Development or configurations:Profile", "platforms:Windows" }
	links { "%{Library.Vulkan}", "%{Library.VulkanShaderC}" }
	
filter { "configurations:Release", "platforms:Windows" }
	links { "%{Library.Vulkan}", "%{Library.VulkanShaderC}" }
end
end

filter "system:prospero"
	systemversion "latest"
	defines { "PLATFORM_PROSPERO" }
	include "./Prospero.lua"
	
filter "system:xboxsx"
	systemversion "latest"
	defines { "PLATFORM_XBOXSX" }
	include "./Xboxsx.lua"
	
filter "platforms:Windows"
	system "windows"
	architecture "x64"
	defines { "PLATFORM_WINDOWS" }
	include "./Windows.lua"
	
filter "platforms:Linux"
	system "linux"
	architecture "x64"
	defines { "PLATFORM_LINUX" }
	include "./Linux.lua"
	
filter "platforms:Mac"
	system "macosx"
	architecture "x64"
	defines { "PLATFORM_MACOS" }
	include "./Macosx.lua"
	
filter "platforms:Prospero"
	system "bsd"
	architecture "x64"
	defines { "PLATFORM_PROSPERO" }
	include "./Prospero.lua"
	
filter "platforms:XboxSeriesX"
	system "windows"
	architecture "x64"
	defines { "PLATFORM_XBOXSX" }
	include "./Xboxsx.lua"

filter "configurations:Debug"
	defines { "DEBUG" }
	runtime "Debug"
	optimize "Off"
	symbols "On"

filter "configurations:Development"
	defines { "DEBUG", "DEVELOPMENT" }
	runtime "Debug"
	optimize "On"
	symbols "On"
	
filter "configurations:Profile"
	defines { "DEBUG", "PROFILE" }
	runtime "Debug"
	optimize "On"
	symbols "On"

filter "configurations:Release"
	defines { "NDEBUG", "RELEASE" }
	runtime "Release"
	optimize "On"
	symbols "Off"
