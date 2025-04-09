#include "InputLayout.h"

InputLayout::InputLayout()
{
}

InputLayout::~InputLayout()
{
}

void InputLayout::AppendElement(VertexStream vertexStream)
{
	D3D12_INPUT_ELEMENT_DESC element;
	ZeroMemory(&element, sizeof(D3D12_INPUT_ELEMENT_DESC));

	element.InputSlot = static_cast<UINT>(inputElementsList.size());
	element.SemanticIndex = 0;
	element.InstanceDataStepRate = 0;
	element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	element.AlignedByteOffset = 0;

	switch (vertexStream)
	{
	case VertexStream::Position:
		element.SemanticName = "POSITION";
		element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case VertexStream::Color:
		element.SemanticName = "COLOR";
		element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case VertexStream::Normal:
		element.SemanticName = "NORMAL";
		element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case VertexStream::Tangent:
		element.SemanticName = "TANGENT";
		element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case VertexStream::TexCoord:
		element.SemanticName = "TEXCOORD";
		element.Format = DXGI_FORMAT_R32G32_FLOAT;
		break;
	default:
		element.SemanticName = "UNDEFINED";
		element.Format = DXGI_FORMAT_UNKNOWN;
		break;
	}

	inputElementsList.push_back(element);
}

D3D12_INPUT_LAYOUT_DESC InputLayout::GetInputLayoutDesc()
{
	D3D12_INPUT_LAYOUT_DESC inputLayout = {};
	if (inputElementsList.size())
	{
		inputLayout.pInputElementDescs = inputElementsList.data();
		inputLayout.NumElements = static_cast<UINT>(inputElementsList.size());
	}
	return inputLayout;
}