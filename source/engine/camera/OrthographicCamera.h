#pragma once

#include "Camera.h"

class OrthographicCamera : public Camera
{
public:
	OrthographicCamera(DirectX::SimpleMath::Vector3 position);
	void CalculateViewProjectionMatrix() override;
	void SetWidth(float width) { this->width = width; }
	float GetWidth() { return width; }
	void SetHeight(float height) { this->height = height; }
	float GetHeight() { return height; }
};