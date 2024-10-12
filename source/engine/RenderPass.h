#include "vector"

class RenderTarget;

class RenderPass
{
public:
	RenderPass();
	~RenderPass();
	virtual void Prepare();
	virtual void Execute() = 0;
	virtual void Allocate() = 0;
};