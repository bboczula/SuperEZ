#pragma once

#include "Camera.h"

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(float aspectRatio, DirectX::SimpleMath::Vector3 position);
	void CalculateViewProjectionMatrix() override;
};