#include "Free.h"
#include "Camera.h"

Sapphire::FreeCamera::FreeCamera(float aspectRatio, DirectX::SimpleMath::Vector3 position)
{
}

void Sapphire::FreeCamera::MoveForward(float delta)
{
	camera->position += delta * camera->forward;
}

void Sapphire::FreeCamera::MoveBackward(float step)
{
	camera->position += -step * camera->forward;
}

void Sapphire::FreeCamera::MoveRight(float step)
{
	camera->position += step * camera->right;
}

void Sapphire::FreeCamera::MoveLeft(float step)
{
	camera->position += step * camera->right * -1.0f;
}

void Sapphire::FreeCamera::RecalculateBasicVectors(DirectX::SimpleMath::Matrix& cameraRotation)
{
	camera->forward = DirectX::SimpleMath::Vector3::Transform(camera->DEFAULT_FORWARD, cameraRotation);
	camera->forward.Normalize();

	camera->right = DirectX::SimpleMath::Vector3::Transform(camera->DEFAULT_RIGTH, cameraRotation);
	camera->right.Normalize();

	camera->up = DirectX::SimpleMath::Vector3::Transform(camera->DEFAULT_UP, cameraRotation);
	camera->up.Normalize();
}

void Sapphire::FreeCamera::Rotate(float x, float y, float z)
{
	camera->rotation.x += x * 3.1415f / 180.0f;
	camera->rotation.y += y * 3.1415f / 180.0f;
	DirectX::SimpleMath::Matrix cameraRotationX = DirectX::SimpleMath::Matrix::CreateRotationX(camera->rotation.x);
	DirectX::SimpleMath::Matrix cameraRotationY = DirectX::SimpleMath::Matrix::CreateRotationY(camera->rotation.y);
	auto cameraRotation = cameraRotationX * cameraRotationY;
	RecalculateBasicVectors(cameraRotation);
}