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

project "Engine"
    kind "StaticLib"
    language "C++"
	cppdialect "C++20"
	location(LOCATION_DIRECTORY_NAME)
    targetdir "bin/%{cfg.buildcfg}"
	includedirs
	{
		"source/external/d3dx12",
		"source/external/SimpleMath"
	}
    files
	{
		"source/engine/**.h", "source/engine/**.cpp",
		"source/engine/shaders/**.hlsl"
	}
	symbolspath '$(OutDir)$(TargetName).pdb'
	filter "configurations:Final"
		defines { "FINAL" }
		postbuildcommands { "{COPYFILE} %[%{!wks.location}../source/engine/shaders/shaders.hlsl] %[%{!cfg.targetdir}]" }
	filter "configurations:Debug"
		defines { "DEBUG" }
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