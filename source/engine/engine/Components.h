#pragma once
#include <string>

// A simple component to track position, rotation, scale
struct TransformComponent
{
	float position[3];
	float rotation[3];
	float scale[3];
};

// A component to track which mesh to draw
struct GeometryComponent
{
	HMesh meshHandle;
	// later we can add texture IDs, shader IDs, etc.
};

// A component to track material properties
struct MaterialComponent
{
	HTexture textureHandle;
	// later we can add color, reflectivity, etc.
};

// Material to store generic info about entity, like name
struct InfoComponent
{
    std::string name;
};