# SuperEZ 1.6
 A super-easy DirectX12 game engine.

## Demo

https://github.com/user-attachments/assets/df78cf13-c617-4c57-89d9-d10f806dc1aa

## Release Notes
- New Scene Viewer demo project
  - Put all the pieces in order
- Sinmple Sunlight and Shadows
  - Exposed some of the engine functionality to the game project via services
  - Services are Scene, Input and Picker
  - They allow to check input status, which entity was selected and perform basic operations on entities, like rotating or movin them in the world

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
