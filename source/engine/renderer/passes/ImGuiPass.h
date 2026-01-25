#pragma once

#include <string>
#include <Windows.h>
#include <commdlg.h>

#include "../RenderPass.h"

class ImGuiPass : public RenderPass
{
public:
	ImGuiPass();
	void ConfigurePipelineState() override;
	void PostAssetLoad() override;
	void Initialize() override;
	void Update() override;
	void Execute() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
	std::string OpenFileDialog_Win32(HWND owner = NULL);
	HTexture colorCopyTexture; // Texture to copy color data for ImGui rendering
};