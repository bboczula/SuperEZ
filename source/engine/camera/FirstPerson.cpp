#include "FirstPerson.h"
#include "Camera.h"

Sapphire::FirstPersonCamera::FirstPersonCamera(float aspectRatio, DirectX::SimpleMath::Vector3 position)
{
}

void Sapphire::FirstPersonCamera::MoveForward(float delta)
{
	camera->position.z += delta;
}

void Sapphire::FirstPersonCamera::Strafe(float delta)
{
	camera->position.x += delta;
}
