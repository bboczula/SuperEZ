#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera(float aspectRatio, DirectX::SimpleMath::Vector3 position)
	: Camera()
{
	this->aspectRatio = aspectRatio;
	SetPosition(position);
	CalculateViewProjectionMatrix();
}

void PerspectiveCamera::CalculateViewProjectionMatrix()
{
	auto target = position + forward;
	auto view = DirectX::SimpleMath::Matrix::CreateLookAt(position, target, up);
	auto projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane);
	viewProjection = view * projection;
	viewProjection = viewProjection.Transpose();
}
