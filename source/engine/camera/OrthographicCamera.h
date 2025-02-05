#pragma once

#include "Camera.h"

namespace Sapphire
{
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(DirectX::SimpleMath::Vector3 position);
		void CalculateViewProjectionMatrix() override;
	};
}