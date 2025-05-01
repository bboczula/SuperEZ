#pragma once

#include "CameraControler.h"
#include "../../externals/SimpleMath/SimpleMath.h"

class Orbit : public CameraControler
{
public:
	Orbit(Camera* camera);
	~Orbit();
	// Move
	void MoveForward(float step) override;
	void MoveBackward(float step) override;
	void MoveRight(float step) override;
	void MoveLeft(float step) override;
	// Rotate
	void Rotate(float pitchDeltaDegrees, float yawDeltaDegrees, float /*rollDelta*/) override;
	void RecalculateBasisVectors();
	// Custom
	void SetRadius(float radius);
	float GetRadius();
};