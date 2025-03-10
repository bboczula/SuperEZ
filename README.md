# SuperEZ 1.2
 A super-easy DirectX12 game engine.

## Demo

https://github.com/user-attachments/assets/3d5a925d-879f-41e0-9e4b-0d4df87d5d1c

## Release Notes
- Camera Handling
  - Introduced new classes for camera management and camera controllers.
  - Implemented a system to pass camera data efficiently to shaders.
- Basic Camera Unit Tests
  - Added a new Google Test project to support unit testing.
  - Implemented unit tests for both the camera and arcball camera controller.
  - Depth Testing for Render Targets
  - Implemented depth testing support for Render Targets, enhancing rendering accuracy.
- Drawless Render Pass
  - Introduced support for drawless render passes, enabling non-drawing passes like copy passes to function seamlessly.
- Support for PIX Markers
  - Added PIX markers support to improve debugging and performance analysis within PIX.
- Basic Shader Pipeline
  - Refactored the shader creation process, extracting it from the Render Context for better modularity and maintainability.
