# SuperEZ 1.3
 A super-easy DirectX12 game engine.

## Demo

https://github.com/user-attachments/assets/e1b2ba3a-8306-4ba7-aa7c-c7df161eeaac

## Release Notes
- Loading Geometry From File
  - You can now load scenes from Wavefront files
- Flexible Vertex Buffers
  - Each vertex element will end up in separate Vertex Buffer
  - Each Render Pass can pick and chose elements it needs
  - It improves flexibility and allows to easily extend vertex definition
- Resource Handles
  - Now each resource type will have its own handle
  - A handle is a smartes index into the resrouce array, with some basic safety check
- Drawing Multiple Meshes
  - Each Render Pass has access to all the meshes from the scenes
- Input Handling
  - Added a Raw Input handling to support keyboard and mouse
  - Used the Observer patter to easily extend with for instance game pad
- Camera System Rework
  - Merged perspective and orthographic cameras
  - Moved rotation fields to the base class
  - Added Blender-like handling (rotation with middle mouse button and zoom)
  - Added arbitray front, side and top camera position
  - Posibility to smoothly switch between orthographic and perspective camera
- Orbit Camera
  - Fixed a lot of issues (flipping, gimball lock) by switching to spherical coordinates
  - Previously was using Euler angles and rotation matrices, but there were issues
- More info on the DevLog: https://bboczula.pl/2025/07/02/superez-1-3-devlog/

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
