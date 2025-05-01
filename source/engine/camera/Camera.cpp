#include "Camera.h"

Camera::Camera(float aspectRatio, DirectX::SimpleMath::Vector3 position)
	: position(position), rotation(0.0f, 0.0f, 0.0f), aspectRatio(aspectRatio), width(1.0f), height(1.0f),
	forward(DEFAULT_FORWARD), up(DEFAULT_UP),	right(DEFAULT_RIGTH), target(0.0f, 0.0f, 0.0f)
{
	forward = target - position;
	forward.Normalize();
	InitializeYawAndPitchFromPosition();
}

DirectX::SimpleMath::Matrix* Camera::GetViewProjectionMatrixPtr(CameraType type)
{
	CalculateViewProjectionMatrix(type);
	return &viewProjection;
}

void Camera::SetPosition(DirectX::SimpleMath::Vector3 position)
{
	this->position = position;
	InitializeYawAndPitchFromPosition();
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

void Camera::InitializeYawAndPitchFromPosition()
{
	// Step 1: Compute offset from target
	DirectX::SimpleMath::Vector3 offset = position - target;

	auto radius = offset.Length();

	if (radius < 0.0001f)
	{
		// Prevent division by zero if somehow sitting on target
		yaw = 0.0f;
		pitch = 0.0f;
		return;
	}

	// Step 3: Calculate pitch (vertical angle)
	pitch = asinf(offset.y / radius);
	// asin returns value between -90 and +90 degrees (in radians)

	// Step 4: Calculate yaw (horizontal angle)
	yaw = atan2f(offset.x, offset.z);
	// atan2 handles quadrants properly

	// Yaw will be between -pi and +pi, perfectly good for orbit
}

void Camera::CalculateViewProjectionMatrix(CameraType type)
{
	auto target = position + forward;
	switch (type)
	{
	case CameraType::PERSPECTIVE:
		projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane);
		break;
	case CameraType::ORTHOGRAPHIC:
		projection = DirectX::SimpleMath::Matrix::CreateOrthographic(width, height, nearPlane, farPlane);
		break;
	default:
		break;
	}
	view = DirectX::SimpleMath::Matrix::CreateLookAt(position, position + forward, up);
	viewProjection = view * projection;
	viewProjection = viewProjection.Transpose();
}
