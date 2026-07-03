# SuperEZ 1.7
 A super-easy DirectX12 game engine.

## Demo

https://github.com/user-attachments/assets/df78cf13-c617-4c57-89d9-d10f806dc1aa

## Release Notes
- MIP Mapping support
  - MIP Levels are generated on the CPU during asset loading and uploaded to the GPU
  - Added a whole bunch of debug settings for testing (MIP heatmap, bias, force level, etc.)
  - Enabled by default
- Simple Materials
  - Each material has additional specular properties
  - This still uses Blinn-Phong lighting model
- Simple Render Target Viewer
  - Each Render Target can now be viewed in the Viewport
  - There are some transformation taking place to help visualize
- Scene Import Script
  - The script can make it much, much easier to import assets and scenes

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
