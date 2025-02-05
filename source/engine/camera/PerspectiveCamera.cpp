#include "PerspectiveCamera.h"

Sapphire::PerspectiveCamera::PerspectiveCamera(float aspectRatio, DirectX::SimpleMath::Vector3 position)
	: Camera(), aspectRatio(aspectRatio)
{
	CalculateViewProjectionMatrix();
}

void Sapphire::PerspectiveCamera::CalculateViewProjectionMatrix()
{
	auto target = position + forward;
	auto view = DirectX::SimpleMath::Matrix::CreateLookAt(position, target, up);
	auto projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(FOV_IN_RADIANS, aspectRatio, NEAR_PLANE, FAR_PLANE);
	viewProjection = view * projection;
	viewProjection = viewProjection.Transpose();
}
