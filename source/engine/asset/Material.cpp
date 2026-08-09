#include "Material.h"

#include <iostream>

Material::Material(HTexture texIndex, UINT rawHandleOffset, UINT srgbHandleOffset, const CHAR* name)
	: texIndex(texIndex), rawHandleOffset(rawHandleOffset), srgbHandleOffset(srgbHandleOffset)
{
	strcpy_s(this->name, name);
}

Material::~Material()
{
	std::cout << "Material Destructor: " << name << std::endl;
	// Cleanup if necessary
	// For example, release resources or perform other cleanup tasks
}
