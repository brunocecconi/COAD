
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
