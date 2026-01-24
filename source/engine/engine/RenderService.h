#pragma once
#include "Coordinator.h"

class RenderService
{
public:
	void CreateEntity(Coordinator& coordinator, unsigned int id);
	void Update(Coordinator& coordinator);
};