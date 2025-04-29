#include "Orbit.h"
#include "Camera.h"

Orbit::Orbit(Camera* camera) : target(0.0f, 0.0f, 0.0f)
{
	SetCamera(camera);
	camera->forward = target - camera->position;
	camera->forward.Normalize();
}

Orbit::~Orbit()
{
}

void Orbit::MoveForward(float step)
{
}

void Orbit::MoveBackward(float step)
{
}

void Orbit::MoveRight(float step)
{
}

void Orbit::MoveLeft(float step)
{
}

void Orbit::Rotate(float x, float y, float z)
{
	camera->rotation.x = DirectX::XMConvertToRadians(x);
	camera->rotation.y = DirectX::XMConvertToRadians(y);
	camera->rotation.z = DirectX::XMConvertToRadians(z);

	DirectX::SimpleMath::Matrix cameraRotationX = DirectX::SimpleMath::Matrix::CreateRotationX(camera->rotation.x);
	DirectX::SimpleMath::Matrix cameraRotationY = DirectX::SimpleMath::Matrix::CreateRotationY(camera->rotation.y);
	DirectX::SimpleMath::Matrix cameraRotationZ = DirectX::SimpleMath::Matrix::CreateRotationZ(camera->rotation.z);
	auto cameraRotation = cameraRotationX * cameraRotationY * cameraRotationZ;

	// Now we really have to roatet position
	camera->position = DirectX::SimpleMath::Vector3::Transform(camera->position, cameraRotation);

	RecalculateBasisVectors();
}

void Orbit::RecalculateBasisVectors()
{
	// Calculation of forward vector should be easy
	camera->forward = target - camera->position;
	camera->forward.Normalize();

	camera->right = camera->DEFAULT_UP.Cross(camera->forward);
	camera->right.Normalize();
	camera->up = camera->right.Cross(camera->forward);
	camera->up.Normalize();
	camera->up *= -1.0f;
}

void Orbit::SetRadius(float radius)
{
	DirectX::SimpleMath::Vector3 radiusVector = camera->position - target;
	radiusVector.Normalize();
	camera->position = target + (radiusVector * radius);
	camera->forward = target - camera->position;
	camera->forward.Normalize();
}

float Orbit::GetRadius()
{
	DirectX::SimpleMath::Vector3 radiusVector = camera->position - target;
	return radiusVector.Length();
}
