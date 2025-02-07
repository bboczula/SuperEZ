#pragma once

#include "../../externals/SimpleMath/SimpleMath.h"

class Camera
{
	friend class FreeCamera;
	friend class FirstPersonCamera;
	friend class Arcball;
public:
	Camera();
	DirectX::SimpleMath::Matrix* GetViewProjectionMatrixPtr();
	void SetPosition(DirectX::SimpleMath::Vector3 position);
	DirectX::SimpleMath::Vector3 GetPosition();
	void SetRotation(DirectX::SimpleMath::Vector3 rotation);
	DirectX::SimpleMath::Vector3 GetRotation();
	void LogInfo();
protected:
	virtual void CalculateViewProjectionMatrix() = 0;
	const DirectX::SimpleMath::Vector3 DEFAULT_UP = { 0.0f, 1.0f, 0.0f };
	const DirectX::SimpleMath::Vector3 DEFAULT_FORWARD = { 0.0f, 0.0f, -1.0f };
	const DirectX::SimpleMath::Vector3 DEFAULT_RIGTH = { 1.0f, 0.0f, 0.0f };
	// Points
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 rotation;
	// Vectors
	DirectX::SimpleMath::Vector3 up;
	DirectX::SimpleMath::Vector3 right;
	DirectX::SimpleMath::Vector3 forward;
	// Matrices
	DirectX::SimpleMath::Matrix viewProjection;
};