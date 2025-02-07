#include "FirstPerson.h"
#include "Camera.h"

FirstPersonCamera::FirstPersonCamera(float aspectRatio, DirectX::SimpleMath::Vector3 position)
{
}

void FirstPersonCamera::MoveForward(float delta)
{
	camera->position.z += delta;
}

void FirstPersonCamera::Strafe(float delta)
{
	camera->position.x += delta;
}
