#include "Free.h"
#include "Camera.h"
#include <algorithm>

FreeCamera::FreeCamera(Camera* camera)
{
	SetCamera(camera);
}

void FreeCamera::MoveForward(float delta)
{
	camera->position += delta * camera->forward;
}

void FreeCamera::MoveBackward(float step)
{
	camera->position += -step * camera->forward;
}

void FreeCamera::MoveRight(float step)
{
	camera->position += step * camera->right;
}

void FreeCamera::MoveLeft(float step)
{
	camera->position += step * camera->right * -1.0f;
}

void FreeCamera::RecalculateBasicVectors(DirectX::SimpleMath::Matrix& cameraRotation)
{
	camera->forward = DirectX::SimpleMath::Vector3::Transform(camera->DEFAULT_FORWARD, cameraRotation);
	camera->forward.Normalize();

	camera->right = DirectX::SimpleMath::Vector3::Transform(camera->DEFAULT_RIGTH, cameraRotation);
	camera->right.Normalize();

	camera->up = DirectX::SimpleMath::Vector3::Transform(camera->DEFAULT_UP, cameraRotation);
	camera->up.Normalize();
}

void FreeCamera::Rotate(float x, float y, float z)
{
	camera->rotation.x += x * 3.1415f / 180.0f;
	camera->rotation.y += y * 3.1415f / 180.0f;
	DirectX::SimpleMath::Matrix cameraRotationX = DirectX::SimpleMath::Matrix::CreateRotationX(camera->rotation.x);
	DirectX::SimpleMath::Matrix cameraRotationY = DirectX::SimpleMath::Matrix::CreateRotationY(camera->rotation.y);
	auto cameraRotation = cameraRotationX * cameraRotationY;
	RecalculateBasicVectors(cameraRotation);
}