#pragma once

#include <d3d12.h>

class Shader
{
public:
	Shader();
	void Compile(LPCWSTR shaderName, LPCSTR entryPoint, LPCSTR shaderModel);
	ID3D10Blob* GetBlob() const { return blob; }
private:
	ID3DBlob* blob;
};