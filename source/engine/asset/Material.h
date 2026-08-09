#pragma once

#include <Windows.h>
#include <d3d12.h>
#include "Handle.h"

class Material
{
public:
	Material(HTexture texIndex, UINT rawHandleOffset, UINT srgbHandleOffset, const CHAR* name);
	UINT GetHandleOffset(bool useSrgb) const { return useSrgb ? srgbHandleOffset : rawHandleOffset; }
	~Material();
private:
	HTexture texIndex;
	UINT rawHandleOffset;
	UINT srgbHandleOffset;
	CHAR name[32];
};
