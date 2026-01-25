#pragma once
#include "Coordinator.h"

class RenderService
{
public:
	void CreateEntity(Coordinator& coordinator, unsigned int id, std::string name);
	void Update(Coordinator& coordinator);
};