-- premake5.lua

-- Global Variables
LOCATION_DIRECTORY_NAME = "build"

-- Workspace
workspace "SuperEZ"
    configurations
	{
		"Debug",
		"Release"
	}
	system "Windows"
    architecture "x86_64"
	location(LOCATION_DIRECTORY_NAME)
	project "Engine"
	project "Game"

project "Engine"
    kind "StaticLib"
    language "C++"
	cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"
	includedirs
	{
		"source/external/d3dx12"
	}
    files
	{
		"source/engine/**.h", "source/engine/**.cpp"
	}
	
project "Game"
    kind "ConsoleApp"
    language "C++"
	cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"
	links
	{
		"Engine"
	}
    files {
		"source/game/**.h", "source/game/**.cpp"
	}