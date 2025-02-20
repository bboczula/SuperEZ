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
	filter{}
	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"
	filter{}
	filter "configurations:PreRelease"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
	filter{}
	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
	filter{}
	filter "system:Windows"
		buildoptions {
			"-GS", "-W4", "-WX", "-wd4251", "-wd4275", "-nologo", "-J",
			"-D_UNICODE", "-DUNICODE", "-DWIN32", "-D_WIN32",
			"-EHs-c-", "-D_HAS_EXCEPTIONS=0", "-GR-", "-wd4702", "-utf-8"
		}
		defines {
			"GTEST_OS_WINDOWS"
		}
		
project "Engine"
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
		buildcommands  { "{COPYFILE} %[%{!wks.location}../source/externals/PixEvents/bin/**.dll] %[%{!cfg.targetdir}]" } -- This really only applies to Debug builds
		buildoutputs { "%{!cfg.targetdir}/WinPixEventRuntime.dll" } -- Technically creates a dummy file to track execution, but I've never found this file
		buildmessage("Copying the PIX Event runtime...")
		links { "WinPixEventRuntime.lib" }
	filter("files:**.hlsl")
		flags("ExcludeFromBuild")
	
project "Game"
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