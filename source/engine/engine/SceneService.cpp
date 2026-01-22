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
