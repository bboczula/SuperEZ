#pragma once

#include <Windows.h>
#include <d3d12.h>
#include "Handle.h"

class Material
{
public:
	Material(HTexture texIndex, UINT handleOffset, const CHAR* name);
	UINT GetHandleOffset() const { return handleOffset; }
	~Material();
private:
	HTexture texIndex;
	UINT handleOffset;
	CHAR name[32];
};