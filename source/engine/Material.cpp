#include "Material.h"

Material::Material(HTexture texIndex, UINT handleOffset, const CHAR* name)
	: texIndex(texIndex), handleOffset(handleOffset) 
{
	strcpy_s(this->name, name);
}
