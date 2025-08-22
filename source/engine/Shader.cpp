#include "Shader.h"
#include "Utils.h"

#include <d3dcompiler.h>

Shader::Shader() : blob(nullptr)
{
}

void Shader::Compile(LPCWSTR shaderName, LPCSTR entryPoint, LPCSTR shaderModel)
{
	UINT compileFlags = 0;
#if defined(DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// In final we will copy shaders to the bin directory
	ID3DBlob* errorBlob = nullptr;
	HRESULT result = D3DCompileFromFile(shaderName, nullptr, nullptr, entryPoint, shaderModel, compileFlags, 0, &blob, &errorBlob);

	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			printf("Shader compilation error:\n%s\n", (char*)errorBlob->GetBufferPointer());
		}
		else
		{
			char buf[128];
			sprintf_s(buf, "D3DCompileFromFile failed with HRESULT 0x%08X\n", result);
			OutputDebugStringA(buf);
			printf("%s", buf);
		}
		exit(0);
	}
}
