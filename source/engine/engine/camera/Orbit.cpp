#include "Orbit.h"
#include "Camera.h"
#include <algorithm>

Orbit::Orbit(Camera* camera)
{
	SetCamera(camera);
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

void Orbit::Rotate(float pitchDeltaDegrees, float yawDeltaDegrees, float /*rollDelta*/)
{
	// 1. Accumulate yaw and pitch deltas (convert degrees to radians)
	camera->pitch += DirectX::XMConvertToRadians(pitchDeltaDegrees);
	camera->yaw += DirectX::XMConvertToRadians(yawDeltaDegrees);

	// 2. Clamp pitch to [-89°, 89°] to avoid flipping over poles
	const float limit = DirectX::XMConvertToRadians(89.0f);
	camera->pitch = std::clamp(camera->pitch, -limit, limit);

	// 3. Recalculate camera position using spherical coordinates
	auto radius = GetRadius();
	float x = radius * cosf(camera->pitch) * sinf(camera->yaw);
	float y = radius * sinf(camera->pitch);
	float z = radius * cosf(camera->pitch) * cosf(camera->yaw);

	camera->position = camera->target + DirectX::SimpleMath::Vector3(x, y, z);

	RecalculateBasisVectors();
}

void Orbit::RecalculateBasisVectors()
{
	// Calculation of forward vector should be easy
	camera->forward = camera->target - camera->position;
	camera->forward.Normalize();

	camera->right = camera->DEFAULT_UP.Cross(camera->forward);
	camera->right.Normalize();
	camera->up = camera->right.Cross(camera->forward);
	camera->up.Normalize();
	camera->up *= -1.0f;
}

void Orbit::SetRadius(float radius)
{
	DirectX::SimpleMath::Vector3 radiusVector = camera->position - camera->target;
	radiusVector.Normalize();
	camera->position = camera->target + (radiusVector * radius);
	camera->forward = camera->target - camera->position;
	camera->forward.Normalize();
}

float Orbit::GetRadius()
{
	DirectX::SimpleMath::Vector3 radiusVector = camera->position - camera->target;
	return radiusVector.Length();
}
