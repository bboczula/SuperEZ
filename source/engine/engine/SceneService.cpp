#include "SceneService.h"

EntityId SceneService::FindEntityByName(const std::string& name) const
{
      const auto& items = renderContext.GetRenderItems();
      for (const RenderItem& item : items)
      {
            // name[] is a fixed char array on RenderItem
            if (std::strcmp(item.name, name.c_str()) == 0)
                  return item.id;
      }

      return InvalidEntity;
}

void SceneService::SetPosition(EntityId id, Vec3 p)
{
      RenderItem* item = renderContext.GetRenderItemById(id);
      if (!item) return;

      item->position = DirectX::SimpleMath::Vector3(p.x, p.y, p.z);
}

Vec3 SceneService::GetPosition(EntityId id) const
{
      const RenderItem* item = renderContext.GetRenderItemById(id);
      if (!item) return { 0,0,0 };

      // For a pure translation matrix, this is valid in SimpleMath.
      return { item->position.x, item->position.y, item->position.z };
}

Vec3 SceneService::GetRotationEuler(EntityId id) const
{
      const RenderItem* item = renderContext.GetRenderItemById(id);
      if (!item)
            return { 0, 0, 0 };

      constexpr float RadToDeg = 180.0f / DirectX::XM_PI;

      return {
          item->rotation.x * RadToDeg,
          item->rotation.y * RadToDeg,
          item->rotation.z * RadToDeg
      };
}

void SceneService::SetRotationEuler(EntityId id, Vec3 degrees)
{
      RenderItem* item = renderContext.GetRenderItemById(id);
      if (!item) return;

      constexpr float DegToRad = DirectX::XM_PI / 180.0f;

      item->rotation = DirectX::SimpleMath::Vector3(
            degrees.x * DegToRad,
            degrees.y * DegToRad,
            degrees.z * DegToRad
      );
}

Vec3 SceneService::GetScale(EntityId id) const
{
      const RenderItem* item = renderContext.GetRenderItemById(id);
      if (!item) return { 0,0,0 };

      return { item->scale.x, item->scale.y, item->scale.z };
}

void SceneService::SetScale(EntityId id, Vec3 scale)
{
      RenderItem* item = renderContext.GetRenderItemById(id);
      if (!item) return;

      item->scale = DirectX::SimpleMath::Vector3(scale.x, scale.y, scale.z);
}
