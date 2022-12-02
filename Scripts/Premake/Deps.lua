
LibraryNameFromConfig = {};

function SetLibDirs(ProjectName)

if ProjectName == "COAD" then

filter { "configurations:Debug or configurations:Development or configurations:Profile or configurations:DebugTool or configurations:DevelopmentTool or configurations:ProfileTool", "platforms:Windows" }
	libdirs { "./Deps/Windows/debug/lib" }
	links {"mimalloc-debug", "EASTL" }
	
filter { "configurations:Release or configurations:ReleaseTool", "platforms:Windows" }
	libdirs { "./Deps/Windows/lib" }
	links {"mimalloc", "EASTL" }

elseif ProjectName == "COAD-Cmd" then

filter { "configurations:Debug or configurations:Development or configurations:Profile or configurations:DebugTool or configurations:DevelopmentTool or configurations:ProfileTool", "platforms:Windows" }
	libdirs { "./Deps/Windows/debug/lib" }
	links { "COAD", "mimalloc-debug", "EASTL", "%{Library.Vulkan}" }
	postbuildcommands { "{COPY} ..\\Deps\\Windows\\debug\\bin\\*.dll %{cfg.targetdir}" }
	
filter { "configurations:Release or configurations:ReleaseTool", "platforms:Windows" }
	libdirs { "./Deps/Windows/lib" }
	links { "COAD", "mimalloc", "EASTL", "%{Library.Vulkan}" }
	postbuildcommands { "{COPY} ..\\Deps\\Windows\\bin\\*.dll %{cfg.targetdir}"}
	
elseif ProjectName == "COAD-Benchmark" then

bindir = "..\\Bin\\%{cfg.buildcfg}-%{cfg.platform}-%{cfg.architecture}\\"

filter { "configurations:Debug or configurations:Development or configurations:Profile or configurations:DebugTool or configurations:DevelopmentTool or configurations:ProfileTool", "platforms:Windows" }
	libdirs { "./Deps/Windows/debug/lib" }
	links { "COAD", "mimalloc-debug", "EASTL", "benchmark", "benchmark_main" }
	postbuildcommands { "{COPY} ..\\Deps\\Windows\\debug\\bin\\*.dll %{cfg.targetdir}" }
	
filter { "configurations:Release or configurations:ReleaseTool", "platforms:Windows" }
	libdirs { "./Deps/Windows/lib" }
	links { "COAD", "mimalloc", "EASTL", "benchmark", "benchmark_main" }
	postbuildcommands { "{COPY} ..\\Deps\\Windows\\bin\\*.dll %{cfg.targetdir}"}

end

end

filter "platforms:Windows"
	IncludeDir["Windows"] = "./Deps/Windows/include"
	
filter "platforms:Linux"
	IncludeDir["Linux"] = "./Deps/Linux/include"
	
filter "platforms:Mac"
	IncludeDir["Mac"] = "./Deps/Macosx/include"
	
filter "platforms:Prospero"
	IncludeDir["Prospero"] = "./Deps/Prospero/include"
	
filter "platforms:XboxSeriesX"
	IncludeDir["XboxSeriesX"] = "./Deps/Xboxsx/include"
