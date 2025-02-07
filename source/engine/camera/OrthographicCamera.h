#pragma once

#include "Camera.h"

class OrthographicCamera : public Camera
{
public:
	OrthographicCamera(DirectX::SimpleMath::Vector3 position);
	void CalculateViewProjectionMatrix() override;
};