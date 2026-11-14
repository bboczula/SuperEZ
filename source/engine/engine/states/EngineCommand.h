#pragma once

#include <variant>
#include "EngineCommandPayloads.h"

enum class EngineCommandType
{
	None,
	Startup,
	LoadAssets,
	GameLoop,
	UnloadAssets,
	AddLibraryAsset,
	DuplicateObject,
	Exit
};

using EngineCommandPayload = std::variant<std::monostate, LoadAssetsPayload, AddLibraryAssetPayload, DuplicateObjectPayload>;

struct EngineCommand
{
	EngineCommandType type;
	EngineCommandPayload payload;
};
