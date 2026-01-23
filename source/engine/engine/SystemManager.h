#pragma once
#include "ECSTypes.h"
#include <set>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cassert>

// 1. The Base System Class
// All your systems (Physics, Render, etc.) inherit from this.
class System {
public:
      // The set of entities this system cares about.
      // e.g., The PhysicsSystem will have a list of all entities with Transform + RigidBody.
      std::set<Entity> mEntities;
};

// 2. The Manager
class SystemManager {
public:
      template<typename T>
      std::shared_ptr<T> RegisterSystem() {
            const char* typeName = typeid(T).name();

            assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

            // Create the system
            auto system = std::make_shared<T>();
            mSystems.insert({ typeName, system });
            return system;
      }

      template<typename T>
      void SetSignature(Signature signature) {
            const char* typeName = typeid(T).name();

            assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

            mSignatures.insert({ typeName, signature });
      }

      void EntityDestroyed(Entity entity) {
            // Erase a destroyed entity from all system lists
            // mEntities is a set so no check needed
            for (auto const& pair : mSystems) {
                  auto const& system = pair.second;
                  system->mEntities.erase(entity);
            }
      }

      // Crucial: When an entity's signature changes (e.g. added a component),
      // update the lists in the systems.
      void EntitySignatureChanged(Entity entity, Signature entitySignature) {
            // Loop through every system
            for (auto const& pair : mSystems) {
                  auto const& type = pair.first;
                  auto const& system = pair.second;
                  auto const& systemSignature = mSignatures[type];

                  // logic: Does this entity match the system's requirements?
                  if ((entitySignature & systemSignature) == systemSignature) {
                        // Yes -> Add to system
                        system->mEntities.insert(entity);
                  }
                  else {
                        // No -> Remove from system (if it was there)
                        system->mEntities.erase(entity);
                  }
            }
      }

private:
      // Map from system type string -> System Pointer
      std::unordered_map<const char*, std::shared_ptr<System>> mSystems;

      // Map from system type string -> Required Component Signature
      std::unordered_map<const char*, Signature> mSignatures;
};