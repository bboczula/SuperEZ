#include <vector>
#include <d3d12.h>

enum class VertexStream
{
	Position,
	Color,
	Normal,
	Tangent,
	TexCoord
};

class InputLayout
{
public:
	InputLayout();
	~InputLayout();
	template <typename T, typename... Types>
	void AppendElementT(T firstArg, Types... restArgs)
	{
		AppendElement(firstArg);
		AppendElementT(restArgs...);
	}
	template<typename T>
	void AppendElementT(T firstArg)
	{
		AppendElement(firstArg);
	}
	D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc();
private:
	void AppendElement(VertexStream vertexStream);
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementsList;
};