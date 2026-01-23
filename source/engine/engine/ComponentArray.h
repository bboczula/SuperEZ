#pragma once
#include "ECSTypes.h"
#include <array>
#include <unordered_map>
#include <cassert>

// The Interface
// We need this because C++ doesn't let us have a list of "ComponentArray<T>" 
// mixed together. So we store pointers to this generic interface instead.
class IComponentArray {
public:
      virtual ~IComponentArray() = default;
      virtual void EntityDestroyed(Entity entity) = 0;
};

// The Implementation
template<typename T>
class ComponentArray : public IComponentArray {
public:
      void InsertData(Entity entity, T component) {
            assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

            // Put new entry at end
            size_t newIndex = mSize;
            mEntityToIndexMap[entity] = newIndex;
            mIndexToEntityMap[newIndex] = entity;
            mComponentArray[newIndex] = component;

            mSize++;
      }

      void RemoveData(Entity entity) {
            assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Removing non-existent component.");

            // Copy element at end into deleted element's place to maintain density
            size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
            size_t indexOfLastElement = mSize - 1;

            // 1. Move the data
            mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

            // 2. Update the map to point to the new spot
            Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
            mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
            mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

            // 3. Clean up the old maps
            mEntityToIndexMap.erase(entity);
            mIndexToEntityMap.erase(indexOfLastElement);

            mSize--;
      }

      T& GetData(Entity entity) {
            assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");

            // Return a reference so we can modify it directly
            return mComponentArray[mEntityToIndexMap[entity]];
      }

      void EntityDestroyed(Entity entity) override {
            if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
                  RemoveData(entity);
            }
      }

private:
      // The packed list of actual component data
      std::array<T, MAX_ENTITIES> mComponentArray;

      // Map from Entity ID -> Array Index
      std::unordered_map<Entity, size_t> mEntityToIndexMap;

      // Map from Array Index -> Entity ID (needed for the "Swap" part of removal)
      std::unordered_map<size_t, Entity> mIndexToEntityMap;

      // Total size of valid entries
      size_t mSize = 0;
};