#pragma once
#include <string>

// A simple component to track position, rotation, scale
struct TransformComponent {
	float position[3];
	float rotation[3];
	float scale[3];
};

// A component to track which mesh to draw
struct MeshComponent {
	std::string meshName;
	// later we can add texture IDs, shader IDs, etc.
};