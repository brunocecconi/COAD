
-- premake5.lua
workspace "COAD"
	architecture "x64"
	configurations { "Debug", "Development", "Profile", "Release", "DebugTool", "DevelopmentTool", "ProfileTool", "ReleaseTool" }
	platforms { "Windows", "Linux", "Mac", "PS5", "XboxSeriesX" }
	startproject "COAD"
	
include "./Scripts/External.lua"
include "./Scripts/Config.lua"	
include "./Scripts/Deps.lua"
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "COAD"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	targetdir "Bin/%{cfg.buildcfg}"
	staticruntime "On"

	files { "Src/**.h", "Src/**.cpp" }

	includedirs
	{
		"%{IncludeDir.deps}",
		"%{IncludeDir.VulkanSDK}"
	}

    links
    {
		LibraryNameFromConfig["Mimalloc"],
		LibraryNameFromConfig["EASTL"]
    }

   targetdir ("../Bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../Int/" .. outputdir .. "/%{prj.name}")
