#include "Camera.h"

Camera::Camera()
	: position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f),
	forward(DEFAULT_FORWARD), up(DEFAULT_UP),	right(DEFAULT_RIGTH)
{
	LogInfo();
}

DirectX::SimpleMath::Matrix* Camera::GetViewProjectionMatrixPtr()
{
	CalculateViewProjectionMatrix();
	return &viewProjection;
}

void Camera::SetPosition(DirectX::SimpleMath::Vector3 position)
{
	this->position = position;
}

DirectX::SimpleMath::Vector3 Camera::GetPosition()
{
	return position;
}

void Camera::SetRotation(DirectX::SimpleMath::Vector3 rotation)
{
	this->rotation = rotation;
}

DirectX::SimpleMath::Vector3 Camera::GetRotation()
{
	return rotation;
}

void Camera::LogInfo()
{
	//Sapphire::Logger::GetInstance().Log("CAMERA:\n");
	//Sapphire::Logger::GetInstance().Log("Position: %f %f %f\n", position.x, position.y, position.z);
	//Sapphire::Logger::GetInstance().Log("Rotation: %f %f %f\n", rotation.x, rotation.y, rotation.z);
	//Sapphire::Logger::GetInstance().Log("Forward: %f %f %f\n", forward.x, forward.y, forward.z);
	//Sapphire::Logger::GetInstance().Log("Up: %f %f %f\n", up.x, up.y, up.z);
	//Sapphire::Logger::GetInstance().Log("Right: %f %f %f\n", right.x, right.y, right.z);
}