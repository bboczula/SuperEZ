#include <Windows.h>

class Engine
{
public:
	Engine();
	~Engine();
	void Initialize();
	void CreateRenderResources();
	void LoadAssets();
	void Tick();
	void Run();
};