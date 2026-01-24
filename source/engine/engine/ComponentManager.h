#pragma once
#include "ECSTypes.h"
#include "ComponentArray.h"
#include <unordered_map>
#include <memory>
#include <typeindex> // For type_info
#include <cassert>

class ComponentManager {
public:
      template<typename T>
      void RegisterComponent() {
            const char* typeName = typeid(T).name();

            assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

            // Add this component type to our signature map
            // (We assign IDs: Transform=0, Mesh=1, etc.)
            mComponentTypes.insert({ typeName, mNextComponentType });

            // Create the ComponentArray pointer
            mComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

            mNextComponentType++;
      }

      template<typename T>
      std::uint8_t GetComponentType() {
            const char* typeName = typeid(T).name();
            assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");
            return mComponentTypes[typeName];
      }

      template<typename T>
      void AddComponent(Entity entity, T component) {
            GetComponentArray<T>()->InsertData(entity, component);
      }

      template<typename T>
      void RemoveComponent(Entity entity) {
            GetComponentArray<T>()->RemoveData(entity);
      }

      template<typename T>
      T& GetComponent(Entity entity) {
            return GetComponentArray<T>()->GetData(entity);
      }

      void EntityDestroyed(Entity entity) {
            // Notify each component array that an entity has been destroyed
            // If it has a component there, it will be removed
            for (auto const& pair : mComponentArrays) {
                  auto const& component = pair.second;
                  component->EntityDestroyed(entity);
            }
      }

private:
      // Helper to get the correct array pointer casted to the right type
      template<typename T>
      std::shared_ptr<ComponentArray<T>> GetComponentArray() {
            const char* typeName = typeid(T).name();
            assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

            return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
      }

      // Map from string pointer (type name) to component type ID
      std::unordered_map<const char*, std::uint8_t> mComponentTypes;

      // Map from string pointer to component array
      std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays;

      // The counter for the next ID to assign
      std::uint8_t mNextComponentType = 0;
};