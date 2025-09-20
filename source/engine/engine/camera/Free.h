#pragma once

#include "CameraControler.h"
#include "../../externals/SimpleMath/SimpleMath.h"

class FreeCamera : public CameraControler
{
public:
	FreeCamera(float aspectRatio, DirectX::SimpleMath::Vector3 position);
	void Rotate(float x, float y, float z) override;
	// Move
	void MoveForward(float delta) override;
	void MoveBackward(float step) override;
	void MoveRight(float step) override;
	void MoveLeft(float step) override;
private:
	void RecalculateBasicVectors(DirectX::SimpleMath::Matrix& cameraRotation);
};