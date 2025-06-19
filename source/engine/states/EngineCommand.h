#pragma once

#include <variant>
#include "EngineCommandPayloads.h"

enum class EngineCommandType
{
	None,
	Startup,
	LoadAssets,
	GameLoop,
	Exit
};

using EngineCommandPayload = std::variant<std::monostate, LoadAssetsPayload>;

struct EngineCommand
{
	EngineCommandType type;
	EngineCommandPayload payload;
};