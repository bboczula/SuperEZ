#include <Windows.h>

class Engine
{
public:
	Engine();
	~Engine();
	void Initialize();
	void CreateRenderResources();
	void Tick();
	void Run();
};