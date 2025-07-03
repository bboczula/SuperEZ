# SuperEZ 1.4
 A super-easy DirectX12 game engine.

## Demo

TODO

## Release Notes
- Simple Texturing
  - Each object is now rendered with a corresponding albedo texture
  - Textures are being loaded from files on hard drive and uploaded to GPU
  - Extended the resource binding mechanism
- Scene Reloading
  - You can switch scenes in the real-time
	   - Added the State Machine to the Engine to support this
  - There is a new menu bar that allows you to select a scene file to load
- ImGui Support
  - Now the engine support the ImGui as GitHub submodule
	   - Added a separate Render Pass and Input Listener
  - I've added a side panel that lists all of the game objects
	   - Once selected, it will display some details about the object
- Basic Scene Graph
  - Scenes are now loaded from XML scene description file
	   - Each entry has a name, geometry and a texture
- Shader Wrapper Class
  - Quality of life improvement
	 - Got rid of another raw DirectX class in favor of the abstraction
- More info on the DevLog: TODO

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
