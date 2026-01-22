#pragma once
#include <cstdint>
#include <SimpleMath.h>
#include "../asset/Handle.h"

struct RenderItem
{
	uint32_t id;
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f };
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f };
	DirectX::SimpleMath::Vector3 scale = { 1.0f, 1.0f, 1.0f };
	HMesh mesh;
	HTexture texture;
	char name[32];
	DirectX::SimpleMath::Matrix World() const
	{
		return DirectX::SimpleMath::Matrix::CreateScale(scale) *
			   DirectX::SimpleMath::Matrix::CreateRotationX(rotation.x) *
			   DirectX::SimpleMath::Matrix::CreateRotationY(rotation.y) *
			   DirectX::SimpleMath::Matrix::CreateRotationZ(rotation.z) *
			   DirectX::SimpleMath::Matrix::CreateTranslation(position);
	}
};
