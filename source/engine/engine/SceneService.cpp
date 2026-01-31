#include "SceneService.h"
#include "Components.h"

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

EntityId SceneService::GetSelectedEntity() const
{
	return renderContext.GetSelectedObjectId();
}

void SceneService::SetPosition(EntityId id, Vec3 p)
{
      auto& transform = coordinator->GetComponent<TransformComponent>(id);
	transform.position[0] = p.x;
	transform.position[1] = p.y;
	transform.position[2] = p.z;
}

Vec3 SceneService::GetPosition(EntityId id) const
{
      const auto& transform = coordinator->GetComponent<TransformComponent>(id);
      return { transform.position[0], transform.position[1], transform.position[2] };
}

Vec3 SceneService::GetRotationEuler(EntityId id) const
{
      const auto& transform = coordinator->GetComponent<TransformComponent>(id);
	return { transform.rotation[0], transform.rotation[1], transform.rotation[2] };
}

void SceneService::SetRotationEuler(EntityId id, Vec3 degrees)
{
      auto& transform = coordinator->GetComponent<TransformComponent>(id);
	transform.rotation[0] = degrees.x;
	transform.rotation[1] = degrees.y;
	transform.rotation[2] = degrees.z;
}

Vec3 SceneService::GetScale(EntityId id) const
{
      const auto& transform = coordinator->GetComponent<TransformComponent>(id);
      return { transform.scale[0], transform.scale[1], transform.scale[2] };
}

void SceneService::SetScale(EntityId id, Vec3 scale)
{
      auto& transform = coordinator->GetComponent<TransformComponent>(id);
      transform.scale[0] = scale.x;
      transform.scale[1] = scale.y;
      transform.scale[2] = scale.z;
}

void SceneService::TweenPositionTo(EntityId id, const Vec3& target, float durationSeconds, Ease ease, TweenCallback onComplete)
{
	// Snap to position immediately for now
	SetPosition(id, target);
}

bool SceneService::IsTweeningPosition(EntityId id) const
{
      return false;
}

void SceneService::CancelPositionTween(EntityId id)
{
}

void SceneService::Update(float dtSeconds)
{
}
