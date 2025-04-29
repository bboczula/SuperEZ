#pragma once

#include "../../externals/SimpleMath/SimpleMath.h"

class Camera
{
	friend class FreeCamera;
	friend class FirstPersonCamera;
	friend class Orbit;
public:
	enum class CameraType
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	};
	Camera(float aspectRatio, DirectX::SimpleMath::Vector3 position);
	DirectX::SimpleMath::Matrix* GetViewProjectionMatrixPtr(CameraType type);
	void SetPosition(DirectX::SimpleMath::Vector3 position);
	DirectX::SimpleMath::Vector3 GetPosition();
	void SetRotation(DirectX::SimpleMath::Vector3 rotation);
	DirectX::SimpleMath::Vector3 GetRotation();
	void CalculateViewProjectionMatrix(CameraType type);
	void SetWidth(float width) { this->width = width; }
	float GetWidth() { return width; }
	void SetHeight(float height) { this->height = height; }
	float GetHeight() { return height; }
	void SetPitch(float pitch) { this->pitch = pitch; }
	float GetPitch() { return DirectX::XMConvertToDegrees(pitch); }
	void SetYaw(float yaw) { this->yaw = yaw; }
	float GetYaw() { return DirectX::XMConvertToDegrees(yaw); }
	void SetRoll(float roll) { this->roll = roll; }
	float GetRoll() { return DirectX::XMConvertToDegrees(roll); }
	// Helper Functions
	void InitializeYawAndPitchFromPosition();
protected:
	// Devault Vectors
	const DirectX::SimpleMath::Vector3 DEFAULT_UP = { 0.0f, 1.0f, 0.0f };
	const DirectX::SimpleMath::Vector3 DEFAULT_FORWARD = { 0.0f, 0.0f, 1.0f };
	const DirectX::SimpleMath::Vector3 DEFAULT_RIGTH = { 1.0f, 0.0f, 0.0f };
	// Points
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 rotation;
	DirectX::SimpleMath::Vector3 target;
	// Vectors
	DirectX::SimpleMath::Vector3 up;
	DirectX::SimpleMath::Vector3 right;
	DirectX::SimpleMath::Vector3 forward;
	// Matrices
	DirectX::SimpleMath::Matrix viewProjection;
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
	// Projection
	float fov = DirectX::XMConvertToRadians(36.0f);
	float aspectRatio;
	// Orthographic
	float width;
	float height;
	// Common
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};