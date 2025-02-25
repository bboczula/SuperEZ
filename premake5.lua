-- premake5.lua

-- Global Variables
LOCATION_DIRECTORY_NAME = "build"

-- Workspace
workspace "SuperEZ"
    configurations { "Debug", "Release", "Final" }
	architecture "x86_64"
	system "Windows"
	location(LOCATION_DIRECTORY_NAME)
	project "Engine"
	project "Game"
	
project "GoogleTest"
	staticruntime "on" --This will set the /MT in Visual Studio and not the default /MD (which stands for DLL)
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	targetdir "bin/%{cfg.buildcfg}"
	files
	{
		"source/externals/googletest/googletest/src/**.h",
		"source/externals/googletest/googletest/src/**.cc"
	}
	includedirs
	{
		"source/externals/googletest/googletest/include",
		"source/externals/googletest/googletest" -- For file src/gtest-internal-inl.h
	}
	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"
	filter "configurations:PreRelease"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
	filter "system:Windows"
		buildoptions {
			"-GS", "-W4", "-WX", "-wd4251", "-wd4275", "-nologo", "-J",
			"-D_UNICODE", "-DUNICODE", "-DWIN32", "-D_WIN32",
			"-EHs-c-", "-D_HAS_EXCEPTIONS=0", "-GR-", "-wd4702", "-utf-8"
		}
		defines {
			"GTEST_OS_WINDOWS"
		}
		
project "UnitTest"
	staticruntime "on"
	kind "ConsoleApp"
	cppdialect "C++17"
	files
	{
		"source/tests/**.h",
		"source/tests/**.cpp"
	}
	includedirs
	{
		"source/externals/googletest/googletest/include"
	}
	links { "GoogleTest", "Engine" }
	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"
	filter "configurations:PreRelease"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
		
project "Engine"
	staticruntime "on" --This will set the /MT in Visual Studio and not the default /MD (which stands for DLL)
    kind "StaticLib"
    language "C++"
	cppdialect "C++20"
	location(LOCATION_DIRECTORY_NAME)
    targetdir "bin/%{cfg.buildcfg}"
	includedirs
	{
		"source/external/d3dx12",
		"source/external/SimpleMath",
		"source/externals/PixEvents/include"
	}
	libdirs
	{
		"source/externals/PixEvents/lib"
	}
    files
	{
		"source/engine/**.h", "source/engine/**.cpp",
		"source/engine/shaders/**.hlsl"
	}
	symbolspath '$(OutDir)$(TargetName).pdb'
	filter "configurations:Final"
		defines { "FINAL" }
		buildcommands  { "{COPYFILE} %[%{!wks.location}../source/engine/shaders/**.hlsl] %[%{!cfg.targetdir}]" } -- Runs before the compilation		
		buildoutputs { "%{cfg.targetdir}/.timestamp" } -- Technically creates a dummy file to track execution, but I've never found this file
	filter "configurations:Debug"
		defines { "DEBUG" }
		runtime "Debug"
		buildcommands  { "{COPYFILE} %[%{!wks.location}../source/externals/PixEvents/bin/**.dll] %[%{!cfg.targetdir}]" } -- This really only applies to Debug builds
		buildoutputs { "%{!cfg.targetdir}/WinPixEventRuntime.dll" } -- Technically creates a dummy file to track execution, but I've never found this file
		buildmessage("Copying the PIX Event runtime...")
		links { "WinPixEventRuntime.lib" }
	filter "configurations:PreRelease"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
	filter("files:**.hlsl")
		flags("ExcludeFromBuild")
	
project "Game"
	staticruntime "on" --This will set the /MT in Visual Studio and not the default /MD (which stands for DLL)
    kind "ConsoleApp"
    language "C++"
	cppdialect "C++20"
	location(LOCATION_DIRECTORY_NAME)
    targetdir "bin/%{cfg.buildcfg}"
	links
	{
		"Engine"
	}
    files {
		"source/game/**.h", "source/game/**.cpp"
	}
	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"
	filter "configurations:PreRelease"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"