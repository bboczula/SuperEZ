#include "TestPass.h"

TestPass::TestPass()
{
}

TestPass::~TestPass()
{
}

void TestPass::Prepare()
{
	// Why do we reeally need Prepare?
}

void TestPass::Execute()
{
	// Then in execute we need to bind the render target
}

void TestPass::Allocate(DeviceContext* deviceContext)
{
	// We want our Render Pass
}