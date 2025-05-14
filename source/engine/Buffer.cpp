#include "Buffer.h"

Buffer::Buffer(ID3D12Resource* resource, D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout, CHAR* name) : resource(resource), layout(layout)
{
	strcpy_s(this->name, name);
}
