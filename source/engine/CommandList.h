#include <d3d12.h>

class CommandList
{
public:
	CommandList();
	~CommandList();
	void Create();
	void Reset(ID3D12PipelineState* pso);
	void Close();
	ID3D12GraphicsCommandList* GetCommandList() { return commandList; }
private:
	ID3D12CommandAllocator* commandAllocator;
	ID3D12GraphicsCommandList* commandList;
};