#pragma once
#include "Coordinator.h"
#include "../renderer/RenderItem.h"

class RenderService
{
public:
	void CreateEntity(Coordinator& coordinator, RenderItem& renderItem);
	void Update(Coordinator& coordinator);
};