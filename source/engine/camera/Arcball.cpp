#include "Arcball.h"
#include "Camera.h"

Arcball::Arcball(Camera* camera) : radius(camera->GetPosition().z), target(0.0f, 0.0f, 0.0f)
{
	SetCamera(camera);
	camera->position = target + DirectX::SimpleMath::Vector3(0.0f, 0.0f, radius);
	camera->forward = target - camera->position;
	camera->forward.Normalize();
}

Arcball::~Arcball()
{
}

void Arcball::MoveForward(float step)
{
	radius += 0.1;
	camera->position = target + DirectX::SimpleMath::Vector3(0.0f, 0.0f, radius);
	camera->forward = target - camera->position;
	camera->forward.Normalize();
}

void Arcball::MoveBackward(float step)
{
	radius -= 0.1;
	camera->position = target + DirectX::SimpleMath::Vector3(0.0f, 0.0f, radius);
	camera->forward = target - camera->position;
	camera->forward.Normalize();
}

void Arcball::MoveRight(float step)
{
}

void Arcball::MoveLeft(float step)
{
}

void Arcball::Rotate(float x, float y, float z)
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

	// Calculation of forward vector should be easy
	camera->forward = target - camera->position;
	camera->forward.Normalize();

	camera->right = camera->DEFAULT_UP.Cross(camera->forward);
	camera->right.Normalize();
	camera->up = camera->right.Cross(camera->forward);
	camera->up.Normalize();
	camera->up *= -1.0f;
}
