#pragma once
#include <cstdint>
#include <SimpleMath.h>
#include "../asset/Handle.h"

struct RenderItem
{
	uint32_t id;
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f };
	HMesh mesh;
	HTexture texture;
	char name[32];
	DirectX::SimpleMath::Matrix World() const
	{
		return DirectX::SimpleMath::Matrix::CreateTranslation(position).Transpose();
	}
};
