# SuperEZ 1.6
 A super-easy DirectX12 game engine.

## Demo

https://github.com/user-attachments/assets/df78cf13-c617-4c57-89d9-d10f806dc1aa

## Release Notes
- Simple Tween Animations
  - Added a tweening API to the Scene Service
  - Game code can now transition entity positions over time
  - This makes interactions like the Slider Puzzle movement smoother and easier to author
- Camera Entities and Editor Hierarchy
  - Added camera entities to the engine/editor flow
  - Improved the editor hierarchy around scene entities
  - Added cleaner ECS reset and cleanup when working with scenes
- Single Sunlight Support
  - Added a Sunlight component for scenes
  - Sunlight direction, color, ambient strength and diffuse strength can be configured
  - Added constant buffer support for sending sunlight data to shaders
  - Added vertex normals support for simple lighting
  - Sunlight settings can be edited from the ImGui editor panel
- Shadow Mapping
  - Added a dedicated Shadow Map render pass and shader
  - Added depth-only pipeline support and typeless depth buffer handling
  - The Forward Pass now samples the shadow map to shade the scene
  - Added configurable shadow bias and slope bias for tuning shadow artifacts
  - Improved sunlight shadow projection using local scene bounds for tighter shadows

## Build
This repository uses the submodules, therefore you need to remember to clone them too.
```
git clone --recurse-submodules <your-repo-url>
```

If you already cloned wihout submodules, you need to do this.
```
git submodule init
git submodule update
```
