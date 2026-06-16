#pragma once

#include <string>
#include <Windows.h>
#include <commdlg.h>

#include "../RenderPass.h"

class RenderPassSettings;;

class ImGuiPass : public RenderPass
{
public:
	ImGuiPass(RenderPassSettings* settings);
	void ConfigurePipelineState() override;
	void PostAssetLoad() override;
	void Initialize() override;
	void Update() override;
	void Execute() override;
	void PostSubmit() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
	std::string OpenFileDialog_Win32(HWND owner = NULL);
	void DrawRenderPassSettingsWindow(RenderPassSettings* settings);
	HTexture colorCopyTexture; // Texture to copy color data for ImGui rendering
	RenderPassSettings* settings = nullptr;
};
