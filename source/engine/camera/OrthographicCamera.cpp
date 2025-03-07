#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(DirectX::SimpleMath::Vector3 position)	: Camera()
{
	SetPosition(position);
	CalculateViewProjectionMatrix();
}

void OrthographicCamera::CalculateViewProjectionMatrix()
{
	float aspectRatio = 1.0f; // 800.0f / 600.0f;
	float distanceToPlane = 2.0f;
	float fov = DirectX::XMConvertToRadians(36.0f);
	height = tan(fov * 0.5f) * distanceToPlane;
	width = height * aspectRatio;

	auto view = DirectX::SimpleMath::Matrix::CreateLookAt(position, forward, up);
	auto projection = DirectX::SimpleMath::Matrix::CreateOrthographic(width * 2.0f, height * 2.0f, 0.1f, 1000.0f);
	viewProjection = view * projection;
	viewProjection = viewProjection.Transpose();
}
