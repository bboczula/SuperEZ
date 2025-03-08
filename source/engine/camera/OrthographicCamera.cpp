#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(DirectX::SimpleMath::Vector3 position) : Camera(), width(1.0f), height(1.0f)
{
	SetPosition(position);
	CalculateViewProjectionMatrix();
}

void OrthographicCamera::CalculateViewProjectionMatrix()
{
	auto view = DirectX::SimpleMath::Matrix::CreateLookAt(position, forward, up);
	auto projection = DirectX::SimpleMath::Matrix::CreateOrthographic(width, height, 0.1f, 1000.0f);
	viewProjection = view * projection;
	viewProjection = viewProjection.Transpose();
}
