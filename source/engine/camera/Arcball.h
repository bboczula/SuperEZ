#pragma once

#include "CameraControler.h"
#include "../../externals/SimpleMath/SimpleMath.h"

class Arcball : public CameraControler
{
public:
	Arcball(Camera* camera);
	~Arcball();
	// Move
	void MoveForward(float step) override;
	void MoveBackward(float step) override;
	void MoveRight(float step) override;
	void MoveLeft(float step) override;
	// Rotate
	void Rotate(float x, float y, float z) override;
private:
	DirectX::SimpleMath::Vector3 target;
	float radius;
};