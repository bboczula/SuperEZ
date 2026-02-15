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
	// For now, let's clear immediatey
	m_positionTweens.clear();

      // ... and add a new one
	PositionTween tween =
      {
            .entity = id,
            .start = GetPosition(id),
            .target = target,
            .elapsed = 0.0f,
            .duration = durationSeconds,
            .ease = ease,
		.onComplete = onComplete
      };

	// First check if there's an existing tween for this entity
	// If so, immediately return (no overlapping tweens for now)
      if (IsTweeningPosition(id))
      {
            return;
      }

	// Add new tween
	m_positionTweens.push_back(tween);

	// And immediately snap to target for now
	SetPosition(id, target);
}

bool SceneService::IsTweeningPosition(EntityId id) const
{
      for (const auto& tween : m_positionTweens)
      {
            if (tween.entity == id)
                  return true;
      }
      return false;
}

void SceneService::CancelPositionTween(EntityId id)
{
}

void SceneService::Update(float dtSeconds)
{
      for(auto& tween : m_positionTweens)
      {
            tween.elapsed += dtSeconds;
            float t = tween.elapsed / tween.duration;
            if (t > 1.0f)
            {
                  t = 1.0f;
            }
            // Simple linear easing for now
            Vec3 newPos;
            newPos.x = tween.start.x + (tween.target.x - tween.start.x) * t;
            newPos.y = tween.start.y + (tween.target.y - tween.start.y) * t;
            newPos.z = tween.start.z + (tween.target.z - tween.start.z) * t;
            SetPosition(tween.entity, newPos);
            // Check for completion
            if(t >= 1.0f && tween.onComplete)
            {
			// remove tween
                  for(auto it = m_positionTweens.begin(); it != m_positionTweens.end(); ++it)
                  {
                        if(it->entity == tween.entity)
                        {
                              m_positionTweens.erase(it);
                              break;
                        }
			}
                  m_pendingTweenCallbacks.emplace_back(tween.onComplete, tween.entity);
            }
	}
}
