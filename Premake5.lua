
-- premake5.lua
workspace "COAD"
	architecture "x64"
	configurations { "Debug", "Development", "Profile", "Release" }
	platforms { "Windows", "Linux", "Mac", "Prospero", "XboxSeriesX" }
	startproject "COAD-Cmd"
	
include "./Scripts/Premake/External.lua"
include "./Scripts/Premake/Config.lua"	
include "./Scripts/Premake/Deps.lua"

OutputDir = "%{cfg.buildcfg}-%{cfg.platform}-%{cfg.architecture}/"

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
	targetdir("./Lib/" .. OutputDir)
	objdir("./Int/" .. OutputDir)
	platforms { "Windows", "Linux", "Mac", "Prospero", "XboxSeriesX" }
	vectorextensions "SSE2"
	warnings "Extra"
	floatingpoint "Fast"
	flags { "MultiProcessorCompile", "FatalWarnings" }

	defines { 
		"_CRT_SECURE_NO_WARNINGS", 
		"USE_SAFE_PTR=1", 
		"USE_EXPLICIT=1", 
		"USE_RESULT=1", 
		"EASTL_EASTDC_VSNPRINTF=0",
		"PRINT_ALLOCATIONS=0", 
		"PRINT_DEALLOCATIONS=0"
	}

	includedirs
	{
		"./Src/",
		IncludeDir["Windows"]
	}
	
	files { "Src/**.h", "Src/**.hpp", "Src/**.cpp", "./Natvis/**.natvis" }
	removefiles { "Src/Cmd/Main.cpp", "Src/Benchmark/Entity.cpp", "Src/Benchmark/Meta.cpp" }
	
	filter { "not files:**.unity.*" }
		flags {"ExcludeFromBuild"}
	
	SetLibDirs("COAD")
	VulkanEnable("COAD")
	EditorEnable()
	
project "COAD-Cmd"
	location "./Int/"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir("./Bin/" .. OutputDir)
	objdir("./Int/" .. OutputDir)
	--staticruntime "On"
	platforms { "Windows", "Linux", "Mac", "Prospero", "XboxSeriesX" }
	vectorextensions "SSE2"
	warnings "Extra"
	floatingpoint "Fast"
	flags { "MultiProcessorCompile", "FatalWarnings" }
	
	defines { 
		"_CRT_SECURE_NO_WARNINGS", 
		"USE_SAFE_PTR=1", 
		"USE_EXPLICIT=1", 
		"EASTL_EASTDC_VSNPRINTF=0"
	}
	
	files { "Src/Cmd/Main.cpp", "./Natvis/**.natvis" }

	includedirs
	{
		"./Src/",
		IncludeDir["Windows"]
	}
	
	SetLibDirs("COAD-Cmd")
	VulkanEnable("COAD-Cmd")
	EditorEnable()

project "COAD-Benchmark-Entity"
	location "./Int/"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir("./Bin/" .. OutputDir)
	objdir("./Int/" .. OutputDir)
	--staticruntime "On"
	platforms { "Windows", "Linux", "Mac", "Prospero", "XboxSeriesX" }
	vectorextensions "SSE2"
	warnings "Extra"
	floatingpoint "Fast"
	flags { "MultiProcessorCompile", "FatalWarnings" }
	
	defines { 
		"_CRT_SECURE_NO_WARNINGS", 
		"USE_SAFE_PTR=1", 
		"USE_EXPLICIT=1", 
		"EASTL_EASTDC_VSNPRINTF=0" 
	}
	
	files { "Src/Benchmark/Entity.cpp", "./Natvis/**.natvis" }

	includedirs
	{
		"./Src/",
		IncludeDir["Windows"],
		IncludeDir["Linux"],
		IncludeDir["Mac"],
		IncludeDir["Prospero"],
		IncludeDir["XboxSeriesX"]
	}
	
	SetLibDirs("COAD-Benchmark")

project "COAD-Benchmark-Meta"
	location "./Int/"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir("./Bin/" .. OutputDir)
	objdir("./Int/" .. OutputDir)
	--staticruntime "On"
	platforms { "Windows", "Linux", "Mac", "Prospero", "XboxSeriesX" }
	vectorextensions "SSE2"
	warnings "Extra"
	floatingpoint "Fast"
	flags { "MultiProcessorCompile", "FatalWarnings" }
	
	defines { 
		"_CRT_SECURE_NO_WARNINGS", 
		"USE_SAFE_PTR=1", 
		"USE_EXPLICIT=1", 
		"EASTL_EASTDC_VSNPRINTF=0" 
	}
	
	files { "Src/Benchmark/Meta.cpp", "./Natvis/**.natvis" }

	includedirs
	{
		"./Src/",
		IncludeDir["Windows"],
		IncludeDir["Linux"],
		IncludeDir["Mac"],
		IncludeDir["Prospero"],
		IncludeDir["XboxSeriesX"]
	}
	
	SetLibDirs("COAD-Benchmark")
