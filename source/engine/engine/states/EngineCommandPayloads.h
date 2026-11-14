#pragma once
#include <string>
#include <cstdint>

struct AddLibraryAssetPayload
{
	std::string meshName;
	std::string textureName;
	uint64_t sceneGeneration;
};

struct LoadAssetsPayload
{
	std::string sceneName;
};

struct DuplicateObjectPayload
{
	uint32_t entity;
	uint64_t sceneGeneration;
};
