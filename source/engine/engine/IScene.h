// IScene.h
#pragma once
#include <cstdint>
#include <string>

      using EntityId = uint32_t;
      static constexpr EntityId InvalidEntity = 0;

      struct Vec3
      {
            float x, y, z;
      };

      class IScene
      {
      public:
            virtual ~IScene() = default;

            // Lookup (use in OnInit only; do NOT spam this every frame)
            virtual EntityId FindEntityByName(const std::string& name) const = 0;

            // Transform - minimal for now
            virtual Vec3 GetPosition(EntityId id) const = 0;
            virtual void SetPosition(EntityId id, Vec3 p) = 0;

            // Optional but cheap and useful (you can implement later)
            // virtual Vec3 GetRotationEuler(EntityId id) const = 0;
            // virtual void SetRotationEuler(EntityId id, Vec3 rads) = 0;
      };
