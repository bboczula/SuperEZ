#pragma once

#include "IScene.h"
#include "../renderer/RenderContext.h"
#include "Coordinator.h"

#include <cstring>   // strcmp
#include <string>

class SceneService final : public IScene
{
public:
      explicit SceneService(RenderContext& rc, Coordinator* coordinator)
            : renderContext(rc), coordinator(coordinator)
      {
      }
      Coordinator* GetCoordinator() { return coordinator; }
      // IScene
      EntityId FindEntityByName(const std::string& name) const override;
      Vec3 GetPosition(EntityId id) const override;
      void SetPosition(EntityId id, Vec3 p) override;
      Vec3 GetRotationEuler(EntityId id) const override;
	void SetRotationEuler(EntityId id, Vec3 rads) override;
	Vec3 GetScale(EntityId id) const override;
	void SetScale(EntityId id, Vec3 scale) override;
private:
      RenderContext& renderContext;
      Coordinator* coordinator;

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
