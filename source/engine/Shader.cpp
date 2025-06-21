#include "Shader.h"
#include "Utils.h"

#include <d3dcompiler.h>

Shader::Shader() : blob(nullptr)
{
}

void Shader::Compile(LPCWSTR shaderName, LPCSTR entryPoint, LPCSTR shaderModel)
{
#if defined(DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	// In final we will copy shaders to the bin directory
	ExitIfFailed(D3DCompileFromFile(shaderName, nullptr, nullptr, entryPoint, shaderModel, compileFlags, 0, &blob , nullptr));
}
