# SuperEZ 1.7
 A super-easy DirectX12 game engine.

## Demo

https://github.com/user-attachments/assets/8c5c7f4a-9713-4019-b3f9-e112971c820b

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

## Documentation

- [Handling Geometry](docs/handling-geometry.md)
- [Handling Textures](docs/handling-textures.md)
- [Functional Specification](docs/functional-specification/README.md)
- [Asset Import Pipeline](docs/asset-import-pipeline.md)
