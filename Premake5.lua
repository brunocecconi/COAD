
-- premake5.lua
workspace "COAD"
	architecture "x64"
	configurations { "Debug", "Development", "Profile", "Release", "DebugTool", "DevelopmentTool", "ProfileTool", "ReleaseTool" }
	platforms { "Windows", "Linux", "Mac", "Prospero", "XboxSeriesX" }
	startproject "COAD-Cmd"
	
include "./Scripts/Premake/External.lua"
include "./Scripts/Premake/Config.lua"	
include "./Scripts/Premake/Deps.lua"

outputdir = "%{cfg.buildcfg}-%{cfg.platform}-%{cfg.architecture}/"

project "COAD-Premake"
	location "./Int/"
	kind "Utility"
	files { "./Scripts/Premake/**.lua", "./Premake5.lua" }
	
	filter {"platforms:Windows"}
		prebuildcommands { "pushd \"%{wks.location}\"", "Setup.bat", "popd" }

project "COAD"
	location "./Int/"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	targetdir("./Lib/" .. outputdir)
	objdir("./Int/" .. outputdir)
	platforms { "Windows", "Linux", "Mac", "Prospero", "XboxSeriesX" }
	vectorextensions "SSE2"
	warnings "Extra"
	floatingpoint "Fast"
	flags { "MultiProcessorCompile", "FatalWarnings" }
	
	defines { "_CRT_SECURE_NO_WARNINGS", "USE_SAFE_PTR=1", "USE_EXPLICIT=1", "USE_RESULT=1", "EASTL_EASTDC_VSNPRINTF=0" }

	includedirs
	{
		IncludeDir["Windows"],
		IncludeDir["Linux"],
		IncludeDir["Mac"],
		IncludeDir["Prospero"],
		IncludeDir["XboxSeriesX"],
		"./Src/",
		"%{IncludeDir.VulkanSDK}"
	}
	
	files { "Src/**.h", "Src/**.cpp", "Src/**.cpp", "./Natvis/**.natvis" }
	removefiles { "Src/Cmd/Main.cpp", "Src/Benchmark/Entity.cpp", "Src/Benchmark/Meta.cpp" }
	
	filter { "not files:**.unity.*" }
		flags {"ExcludeFromBuild"}
	
	SetLibDirs("COAD")
	
project "COAD-Cmd"
	location "./Int/"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir("./Bin/" .. outputdir)
	objdir("./Int/" .. outputdir)
	--staticruntime "On"
	platforms { "Windows", "Linux", "Mac", "Prospero", "XboxSeriesX" }
	vectorextensions "SSE2"
	warnings "Extra"
	floatingpoint "Fast"
	flags { "MultiProcessorCompile", "FatalWarnings" }
	
	defines { "_CRT_SECURE_NO_WARNINGS", "USE_SAFE_PTR=1", "USE_EXPLICIT=1", "USE_RESULT=1", "EASTL_EASTDC_VSNPRINTF=0" }
	
	files { "Src/Cmd/Main.cpp", "./Natvis/**.natvis" }

	includedirs
	{
		IncludeDir["Windows"],
		IncludeDir["Linux"],
		IncludeDir["Mac"],
		IncludeDir["Prospero"],
		IncludeDir["XboxSeriesX"],
		"./Src/",
		"%{IncludeDir.VulkanSDK}"
	}
	
	SetLibDirs("COAD-Cmd")

project "COAD-Benchmark-Entity"
	location "./Int/"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir("./Bin/" .. outputdir)
	objdir("./Int/" .. outputdir)
	--staticruntime "On"
	platforms { "Windows", "Linux", "Mac", "Prospero", "XboxSeriesX" }
	vectorextensions "SSE2"
	warnings "Extra"
	floatingpoint "Fast"
	flags { "MultiProcessorCompile", "FatalWarnings" }
	
	defines { "_CRT_SECURE_NO_WARNINGS", "USE_SAFE_PTR=1", "USE_EXPLICIT=1", "USE_RESULT=1", "EASTL_EASTDC_VSNPRINTF=0" }
	
	files { "Src/Benchmark/Entity.cpp", "./Natvis/**.natvis" }

	includedirs
	{
		IncludeDir["Windows"],
		IncludeDir["Linux"],
		IncludeDir["Mac"],
		IncludeDir["Prospero"],
		IncludeDir["XboxSeriesX"],
		"./Src/",
		"%{IncludeDir.VulkanSDK}"
	}
	
	SetLibDirs("COAD-Benchmark")

project "COAD-Benchmark-Meta"
	location "./Int/"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir("./Bin/" .. outputdir)
	objdir("./Int/" .. outputdir)
	--staticruntime "On"
	platforms { "Windows", "Linux", "Mac", "Prospero", "XboxSeriesX" }
	vectorextensions "SSE2"
	warnings "Extra"
	floatingpoint "Fast"
	flags { "MultiProcessorCompile", "FatalWarnings" }
	
	defines { "_CRT_SECURE_NO_WARNINGS", "USE_SAFE_PTR=1", "USE_EXPLICIT=1", "USE_RESULT=1", "EASTL_EASTDC_VSNPRINTF=0" }
	
	files { "Src/Benchmark/Meta.cpp", "./Natvis/**.natvis" }

	includedirs
	{
		IncludeDir["Windows"],
		IncludeDir["Linux"],
		IncludeDir["Mac"],
		IncludeDir["Prospero"],
		IncludeDir["XboxSeriesX"],
		"./Src/",
		"%{IncludeDir.VulkanSDK}"
	}
	
	SetLibDirs("COAD-Benchmark")