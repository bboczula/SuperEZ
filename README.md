# SuperEZ 1.5
 A super-easy DirectX12 game engine.

## Demo

https://github.com/user-attachments/assets/7ef97b01-faf7-4d22-9a43-3f1a2432351f

## Release Notes
- Brand new Slider Puzzle game as a demo
  - Put all the pieces in order
- Basic Engine Interface
  - Exposed some of the engine functionality to the game project via services
  - Services are Scene, Input and Picker
  - They allow to check input status, which entity was selected and perform basic operations on entities, like rotating or movin them in the world
- Simple Entity Component System
  - Each Enity can have a different Components
  - You can query components for different entities
  - Sample components are Transform, Geometry or Material
  - You can modify Entity transformation via this system

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
