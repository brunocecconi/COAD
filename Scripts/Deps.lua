
LibraryNameFromConfig = {};
LibraryNameFromConfig["Debug"] = {}
LibraryNameFromConfig["Development"] = {}
LibraryNameFromConfig["Profile"] = {}
LibraryNameFromConfig["Release"] = {}

filter "platform:Windows"
	IncludeDir["deps"] = "./Windows/include"
	LibraryNameFromConfig["Debug"]["Mimalloc"] = "mimalloc-debug"
	LibraryNameFromConfig["Development"]["Mimalloc"] = "mimalloc-debug"
	LibraryNameFromConfig["Profile"]["Mimalloc"] = "mimalloc-debug"
	LibraryNameFromConfig["Release"]["Mimalloc"] = "mimalloc"
	
	LibraryNameFromConfig["Debug"]["Eastl"] = "EASTL"
	LibraryNameFromConfig["Development"]["Eastl"] = "EASTL"
	LibraryNameFromConfig["Profile"]["Eastl"] = "EASTL"
	LibraryNameFromConfig["Release"]["Eastl"] = "EASTL"
	
	LibraryNameFromConfig["Debug"]["Benchmark"] = "benchmark"
	LibraryNameFromConfig["Development"]["Benchmark"] = "benchmark"
	LibraryNameFromConfig["Profile"]["Benchmark"] = "benchmark"
	LibraryNameFromConfig["Release"]["Benchmark"] = "benchmark"
	
	LibraryNameFromConfig["Debug"]["BenchmarkMain"] = "benchmark_main"
	LibraryNameFromConfig["Development"]["BenchmarkMain"] = "benchmark_main"
	LibraryNameFromConfig["Profile"]["BenchmarkMain"] = "benchmark_main"
	LibraryNameFromConfig["Release"]["BenchmarkMain"] = "benchmark_main"
	
filter "platform:Linux"
	IncludeDir["deps"] = "./Linux/include"
	
filter "platform:Mac"
	IncludeDir["deps"] = "./Macosx/include"
	
filter "platform:Prospero"
	IncludeDir["deps"] = "./Prospero/include"
	
filter "platform:XboxSX"
	IncludeDir["deps"] = "./Xboxsx/include"
