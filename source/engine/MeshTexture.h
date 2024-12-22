#include "Texture.h"

class MeshTexture : public Texture
{
public:
	MeshTexture();
	void Create() override;
	void Upload();
};