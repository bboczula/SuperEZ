#pragma once

#include "Camera.h"

namespace Sapphire
{
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float aspectRatio, DirectX::SimpleMath::Vector3 position);
		void CalculateViewProjectionMatrix() override;
	private:
		const float NEAR_PLANE = 0.01f;
		const float FAR_PLANE = 1000.0f;
		const float FOV_IN_RADIANS = DirectX::XMConvertToRadians(36.0f);
		float aspectRatio;
	};
}