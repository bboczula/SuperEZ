#include "Texture.h"

class RenderTarget : public Texture
{
public:
	RenderTarget();
	void Create() override;
private:
	char name[32];
};