#pragma once

#include "RenderPass.h"

class TrianglePass : public RenderPass
{
public:
	void Execute() override;
	void Allocate() override;
};