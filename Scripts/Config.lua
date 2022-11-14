
filter "system:windows"
	systemversion "latest"
	defines { "PLATFORM_WINDOWS" }
	include "./Windows.lua"
	
filter "system:linux"
	systemversion "latest"
	defines { "PLATFORM_LINUX" }
	include "./Linux.lua"
	
filter "system:macosx"
	systemversion "latest"
	defines { "PLATFORM_MACOS" }
	include "./Macosx.lua"
	
filter "system:prospero"
	systemversion "latest"
	defines { "PLATFORM_PROSPERO" }
	include "./Prospero.lua"
	
filter "system:xboxsx"
	systemversion "latest"
	defines { "PLATFORM_XBOXSX" }
	include "./Xboxsx.lua"
	
filder "platform:Windows"
	system "windows"
	architecture "x64"
	
filder "platform:Linux"
	system "linux"
	architecture "x64"
	
filder "platform:Mac"
	system "macosx"
	architecture "x64"
	
filder "platform:Prospero"
	system "prospero"
	architecture "x64"
	
filder "platform:XboxSX"
	system "xboxsx"
	architecture "x64"

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

filter "configurations:DebugTool"
	defines { "DEBUG", "TOOL" }
	runtime "Debug"
	optimize "Off"
	symbols "On"

filter "configurations:DevelopmentTool"
	defines { "DEBUG", "DEVELOPMENT", "TOOL" }
	runtime "Debug"
	optimize "On"
	symbols "On"
	
filter "configurations:ProfileTool"
	defines { "DEBUG", "PROFILE", "TOOL" }
	runtime "Debug"
	optimize "On"
	symbols "On"

filter "configurations:ReleaseTool"
	defines { "NDEBUG", "RELEASE", "TOOL" }
	runtime "Release"
	optimize "On"
	symbols "Off"
	