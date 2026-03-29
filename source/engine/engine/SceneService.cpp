#include "SceneService.h"

#include "Components.h"
#include "camera/Camera.h"

namespace {
DirectX::SimpleMath::Vector3 ToSimpleMath(Vec3 value)
{
      return { value.x, value.y, value.z };
}

Vec3 ToVec3(const DirectX::SimpleMath::Vector3& value)
{
      return { value.x, value.y, value.z };
}
}

EntityId SceneService::FindEntityByName(const std::string& name) const
{
      for (EntityId entity = 0; entity < MAX_ENTITIES; ++entity)
      {
            if (!HasInfoComponent(entity))
            {
                  continue;
            }

            const auto& info = coordinator->GetComponent<InfoComponent>(entity);
            if (info.name == name)
            {
                  return entity;
            }
      }

      return InvalidEntity;
}

EntityId SceneService::GetSelectedEntity() const
{
      return renderContext.GetSelectedObjectId();
}

bool SceneService::IsCameraEntity(EntityId id) const
{
      return HasCameraComponent(id);
}

EntityId SceneService::GetActiveCameraEntity() const
{
      const std::size_t activeCameraIndex = renderContext.GetActiveCameraIndex();
      for (EntityId entity = 0; entity < MAX_ENTITIES; ++entity)
      {
            const CameraComponent* camera = TryGetCameraComponent(entity);
            if (camera != nullptr && camera->cameraIndex == activeCameraIndex)
            {
                  return entity;
            }
      }

      return InvalidEntity;
}

void SceneService::SetActiveCamera(EntityId id)
{
      CameraComponent* camera = TryGetCameraComponent(id);
      if (camera == nullptr)
      {
            return;
      }

      renderContext.SetActiveCamera(camera->cameraIndex);

      for (EntityId entity = 0; entity < MAX_ENTITIES; ++entity)
      {
            if (CameraComponent* candidate = TryGetCameraComponent(entity))
            {
                  candidate->active = (entity == id);
            }
      }
}

void SceneService::SetPosition(EntityId id, Vec3 p)
{
      if (TransformComponent* transform = TryGetTransformComponent(id))
      {
            transform->position[0] = p.x;
            transform->position[1] = p.y;
            transform->position[2] = p.z;
      }

      if (CameraComponent* camera = TryGetCameraComponent(id))
      {
            renderContext.GetCamera(static_cast<UINT>(camera->cameraIndex))->SetPosition(ToSimpleMath(p));
      }
}

Vec3 SceneService::GetPosition(EntityId id) const
{
      if (const CameraComponent* camera = TryGetCameraComponent(id))
      {
            const auto value = renderContext.GetCamera(static_cast<UINT>(camera->cameraIndex))->GetPosition();
            return ToVec3(value);
      }

      const auto* transform = TryGetTransformComponent(id);
      if (transform == nullptr)
      {
            return {};
      }

      return { transform->position[0], transform->position[1], transform->position[2] };
}

Vec3 SceneService::GetRotationEuler(EntityId id) const
{
      if (const CameraComponent* camera = TryGetCameraComponent(id))
      {
            const auto value = renderContext.GetCamera(static_cast<UINT>(camera->cameraIndex))->GetRotation();
            return ToVec3(value);
      }

      const auto* transform = TryGetTransformComponent(id);
      if (transform == nullptr)
      {
            return {};
      }

      return { transform->rotation[0], transform->rotation[1], transform->rotation[2] };
}

void SceneService::SetRotationEuler(EntityId id, Vec3 degrees)
{
      if (TransformComponent* transform = TryGetTransformComponent(id))
      {
            transform->rotation[0] = degrees.x;
            transform->rotation[1] = degrees.y;
            transform->rotation[2] = degrees.z;
      }

      if (CameraComponent* camera = TryGetCameraComponent(id))
      {
            renderContext.GetCamera(static_cast<UINT>(camera->cameraIndex))->SetRotation(ToSimpleMath(degrees));
      }
}

Vec3 SceneService::GetScale(EntityId id) const
{
      const auto* transform = TryGetTransformComponent(id);
      if (transform == nullptr)
      {
            return { 1.0f, 1.0f, 1.0f };
      }

      return { transform->scale[0], transform->scale[1], transform->scale[2] };
}

void SceneService::SetScale(EntityId id, Vec3 scale)
{
      if (TransformComponent* transform = TryGetTransformComponent(id))
      {
            transform->scale[0] = scale.x;
            transform->scale[1] = scale.y;
            transform->scale[2] = scale.z;
      }
}

void SceneService::TweenPositionTo(EntityId id, const Vec3& target, float durationSeconds, Ease ease, TweenCallback onComplete)
{
      m_positionTweens.clear();

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

      if (IsTweeningPosition(id))
      {
            return;
      }

      m_positionTweens.push_back(tween);
      SetPosition(id, target);
}

bool SceneService::IsTweeningPosition(EntityId id) const
{
      for (const auto& tween : m_positionTweens)
      {
            if (tween.entity == id)
            {
                  return true;
            }
      }
      return false;
}

void SceneService::CancelPositionTween(EntityId id)
{
}

void SceneService::Update(float dtSeconds)
{
      for (auto& tween : m_positionTweens)
      {
            tween.elapsed += dtSeconds;
            float t = tween.elapsed / tween.duration;
            if (t > 1.0f)
            {
                  t = 1.0f;
            }

            Vec3 newPos;
            newPos.x = tween.start.x + (tween.target.x - tween.start.x) * t;
            newPos.y = tween.start.y + (tween.target.y - tween.start.y) * t;
            newPos.z = tween.start.z + (tween.target.z - tween.start.z) * t;
            SetPosition(tween.entity, newPos);

            if (t >= 1.0f && tween.onComplete)
            {
                  for (auto it = m_positionTweens.begin(); it != m_positionTweens.end(); ++it)
                  {
                        if (it->entity == tween.entity)
                        {
                              m_positionTweens.erase(it);
                              break;
                        }
                  }
                  m_pendingTweenCallbacks.emplace_back(tween.onComplete, tween.entity);
            }
      }
}

bool SceneService::HasInfoComponent(EntityId id) const
{
      Signature sig = coordinator->GetEntityManager()->GetSignature(id);
      return sig.test(coordinator->GetComponentType<InfoComponent>());
}

bool SceneService::HasTransformComponent(EntityId id) const
{
      Signature sig = coordinator->GetEntityManager()->GetSignature(id);
      return sig.test(coordinator->GetComponentType<TransformComponent>());
}

bool SceneService::HasCameraComponent(EntityId id) const
{
      Signature sig = coordinator->GetEntityManager()->GetSignature(id);
      return sig.test(coordinator->GetComponentType<CameraComponent>());
}

CameraComponent* SceneService::TryGetCameraComponent(EntityId id)
{
      return HasCameraComponent(id) ? &coordinator->GetComponent<CameraComponent>(id) : nullptr;
}

const CameraComponent* SceneService::TryGetCameraComponent(EntityId id) const
{
      return HasCameraComponent(id) ? &coordinator->GetComponent<CameraComponent>(id) : nullptr;
}

TransformComponent* SceneService::TryGetTransformComponent(EntityId id)
{
      return HasTransformComponent(id) ? &coordinator->GetComponent<TransformComponent>(id) : nullptr;
}

const TransformComponent* SceneService::TryGetTransformComponent(EntityId id) const
{
      return HasTransformComponent(id) ? &coordinator->GetComponent<TransformComponent>(id) : nullptr;
}
