
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
	
	defines { "USE_SAFE_PTR", "_CRT_SECURE_NO_WARNINGS", "USE_EXPLICIT=1", "USE_RESULT=1" }

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
	
	files { "Src/**.h", "Src/**.cpp", "Src/**.cpp" }
	removefiles { "Src/Cmd/Main.cpp", "Src/Benchmark/Main.cpp" }
	
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
	
	defines { "USE_SAFE_PTR", "_CRT_SECURE_NO_WARNINGS", "USE_EXPLICIT=1", "USE_RESULT=1" }
	
	files { "Src/Cmd/Main.cpp" }

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

project "COAD-Benchmark"
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
	
	defines { "USE_SAFE_PTR", "_CRT_SECURE_NO_WARNINGS" }
	
	files { "Src/Benchmark/Main.cpp" }

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