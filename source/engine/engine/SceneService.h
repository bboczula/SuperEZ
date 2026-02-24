#pragma once

#include "IScene.h"
#include "../renderer/RenderContext.h"
#include "Coordinator.h"

#include <cstring>   // strcmp
#include <string>

class SceneService final : public IScene
{
public:
      using TweenCallback = std::function<void(EntityId)>;
      enum class Ease : uint8_t
      {
            Linear,
            EaseOutQuad,
      };

      struct PositionTween
      {
            EntityId entity = 0;
            Vec3 start{};
            Vec3 target{};
            float elapsed = 0.0f;
            float duration = 0.0f;
            Ease ease = Ease::Linear;
            SceneService::TweenCallback onComplete = nullptr;
      };
      explicit SceneService(RenderContext& rc, Coordinator* coordinator)
            : renderContext(rc), coordinator(coordinator)
      {
      }
      EntityId FindEntityByName(const std::string& name) const override;
	EntityId GetSelectedEntity() const override;
      Vec3 GetPosition(EntityId id) const override;
      void SetPosition(EntityId id, Vec3 p) override;
      Vec3 GetRotationEuler(EntityId id) const override;
	void SetRotationEuler(EntityId id, Vec3 rads) override;
	Vec3 GetScale(EntityId id) const override;
	void SetScale(EntityId id, Vec3 scale) override;
      void TweenPositionTo(EntityId id, const Vec3& target, float durationSeconds, Ease ease = Ease::Linear, TweenCallback onComplete = nullptr);
      bool IsTweeningPosition(EntityId id) const;
      void CancelPositionTween(EntityId id);
      void Update(float dtSeconds);
private:
      RenderContext& renderContext;
      Coordinator* coordinator;
      std::vector<PositionTween> m_positionTweens;
      std::vector<std::pair<SceneService::TweenCallback, EntityId>> m_pendingTweenCallbacks;

      // Helper: read translation from row-major matrix (SimpleMath convention)
      static Vec3 ExtractTranslation(const DirectX::SimpleMath::Matrix& m)
      {
            return Vec3{ m._41, m._42, m._43 };
      }

      // Helper: write translation into row-major matrix
      static void SetTranslation(DirectX::SimpleMath::Matrix& m, Vec3 p)
      {
            m._41 = p.x;
            m._42 = p.y;
            m._43 = p.z;
      }
};
