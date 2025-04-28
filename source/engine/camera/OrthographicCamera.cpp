#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(DirectX::SimpleMath::Vector3 position) : Camera()
{
	width = 1.0f;
	height = 1.0f;
	SetPosition(position);
	CalculateViewProjectionMatrix();
}

void OrthographicCamera::CalculateViewProjectionMatrix()
{
	auto view = DirectX::SimpleMath::Matrix::CreateLookAt(position, forward, up);
	auto projection = DirectX::SimpleMath::Matrix::CreateOrthographic(width, height, nearPlane, farPlane);
	viewProjection = view * projection;
	viewProjection = viewProjection.Transpose();
}
