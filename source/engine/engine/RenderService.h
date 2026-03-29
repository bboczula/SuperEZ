#pragma once
#include "Coordinator.h"
#include "../renderer/RenderItem.h"

class RenderService
{
public:
	Entity CreateEntity(Coordinator& coordinator, RenderItem& renderItem);
	void Update(Coordinator& coordinator);
};
