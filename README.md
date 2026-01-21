# SuperEZ 1.4.1
 A super-easy DirectX12 game engine.

## Demo

https://github.com/user-attachments/assets/06803da2-a7c1-4463-abc6-220b4cb3ee70

## Release Notes
- Editor Object Selection
  - Now when you click an object (either from viewport or ImGUI panel) it gets highlighted 
	 - Used the simples ripple-like highlight pass
- Compute Shader Support
  - Introduced the new Render Pass that represents a compute shader function
  - Added support for Compute Shader compilation, binding and dispatching
- Editor Flythrough Camera
  - Changed the previous Gimbal camera with the Free camera
  	- Much more convenient for editing

## Bug Fixes
- ImGUI messed up after scene reload
  - When you wanted to load new scene, the whole ImGUI looked completely wrong
  - The problem was that I was deleting all the texture resources, but kept the resource views
  - The fix was to split the descriptor heaps into static and dynamic, and only deleting dynamic part during the scene loading

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
