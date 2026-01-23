#pragma once
#include "ECSTypes.h"
#include <queue>
#include <array>
#include <cassert>

class EntityManager {
public:
      EntityManager() {
            // Fill the queue with all possible IDs (0 to 4999)
            for (Entity i = 0; i < MAX_ENTITIES; ++i) {
                  mAvailableEntities.push(i);
            }
      }

      Entity CreateEntity() {
            assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

            // Take the next available ID from the front
            Entity id = mAvailableEntities.front();
            mAvailableEntities.pop();
            mLivingEntityCount++;

            return id;
      }

      void DestroyEntity(Entity entity) {
            assert(entity < MAX_ENTITIES && "Entity out of range.");

            // Reset the signature (forget which components it had)
            mSignatures[entity].reset();

            // Put the ID back in the pool to be reused
            mAvailableEntities.push(entity);
            mLivingEntityCount--;
      }

      void SetSignature(Entity entity, Signature signature) {
            assert(entity < MAX_ENTITIES && "Entity out of range.");
            mSignatures[entity] = signature;
      }

      Signature GetSignature(Entity entity) {
            assert(entity < MAX_ENTITIES && "Entity out of range.");
            return mSignatures[entity];
      }

private:
      std::queue<Entity> mAvailableEntities;
      std::array<Signature, MAX_ENTITIES> mSignatures;
      uint32_t mLivingEntityCount = 0;
};