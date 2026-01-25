#include "RenderService.h"
#include "../renderer/RenderContext.h"
#include "Components.h"

extern RenderContext renderContext;

void RenderService::CreateEntity(Coordinator& coordinator, unsigned int id, std::string name)
{
	// --- NEW ECS CODE ---
		// 1. Create the Entity representation of this object
	Entity newEntity = coordinator.CreateEntity();

	// 2. Add Transform (Default to 0,0,0 for now)
	coordinator.AddComponent(newEntity, TransformComponent{
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 0.0f, 0.0f},
	    {1.0f, 1.0f, 1.0f}
		});

	// 3. Add Geometry Component
	coordinator.AddComponent(newEntity, GeometryComponent{ HMesh(id - 1) });
	coordinator.AddComponent(newEntity, MaterialComponent{ HTexture(id - 1) });
	coordinator.AddComponent(newEntity, InfoComponent{ name });
}

void RenderService::Update(Coordinator& coordinator)
{
      for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
      {
		Signature sig = coordinator.GetEntityManager()->GetSignature(entity);
		bool hasTransform = sig.test(coordinator.GetComponentType<TransformComponent>());
		bool hasGeometry = sig.test(coordinator.GetComponentType<GeometryComponent>());
		bool hasMaterial = sig.test(coordinator.GetComponentType<MaterialComponent>());
		bool hasInfo = sig.test(coordinator.GetComponentType<InfoComponent>());

		if (!hasTransform || !hasGeometry || !hasMaterial || !hasInfo)
			continue;

		assert(entity != 0 && "Entity 0 is usually reserved/not used.");
		assert(entity < renderContext.GetNumOfMeshes() + 1 && "Entity ID exceeds RenderItem count.");

            auto& transform = coordinator.GetComponent<TransformComponent>(entity);
            auto& geo = coordinator.GetComponent<GeometryComponent>(entity);
            auto& mat = coordinator.GetComponent<MaterialComponent>(entity);
		auto& info = coordinator.GetComponent<InfoComponent>(entity);

            // Create the item
            RenderItem item;
            item.id = entity;
            item.position = DirectX::SimpleMath::Vector3(transform.position); // assuming vector math matches
            item.rotation = DirectX::SimpleMath::Vector3(transform.rotation);
            item.scale = DirectX::SimpleMath::Vector3(transform.scale);
            item.mesh = geo.meshHandle;
            item.texture = mat.textureHandle;
		strncpy_s(item.name, info.name.c_str(), _TRUNCATE);

		// Update the RenderItem in the RenderContext (not create)
		size_t renderItemIndex = entity - 1;
		assert(renderItemIndex < renderContext.renderItems.size() && "RenderItem index out of bounds");
		renderContext.renderItems[renderItemIndex] = item;

		// Here, we can further bridge ECS with Renderer Context as needed
		// For example, we might want to set additional flags or update other systems
	}
}