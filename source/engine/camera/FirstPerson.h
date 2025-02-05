#pragma once

#include "CameraControler.h"
#include "../../externals/SimpleMath/SimpleMath.h"

namespace Sapphire
{
	class FirstPersonCamera : public CameraControler
	{
	public:
		FirstPersonCamera(float aspectRatio, DirectX::SimpleMath::Vector3 position);
		void MoveForward(float delta);
		void Strafe(float delta);
	};
}