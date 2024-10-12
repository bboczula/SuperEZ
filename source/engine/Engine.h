class RenderManager;

#include <Windows.h>

class Engine
{
public:
	Engine();
	~Engine();
	void Initialize();
	void Tick();
	void Run();
};