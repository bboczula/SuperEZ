#include "Material.h"

#include <iostream>

Material::Material(HTexture texIndex, UINT handleOffset, const CHAR* name)
	: texIndex(texIndex), handleOffset(handleOffset) 
{
	strcpy_s(this->name, name);
}

Material::~Material()
{
	std::cout << "Material Destructor: " << name << std::endl;
	// Cleanup if necessary
	// For example, release resources or perform other cleanup tasks
}
