#pragma once

#include "Camera.h"

class OrthographicCamera : public Camera
{
public:
	OrthographicCamera(DirectX::SimpleMath::Vector3 position);
	void CalculateViewProjectionMatrix() override;
	float GetWidth() { return width; }
	float GetHeight() { return height; }
private:
	float width;
	float height;
};