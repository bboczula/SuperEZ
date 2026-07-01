#pragma once

#include <string>
#include <Windows.h>
#include <commdlg.h>

#include "../RenderPass.h"
#include "../../engine/Components.h"
#include "../../engine/Coordinator.h"

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
	void DrawInfoComponent(InfoComponent& info);
	void DrawTransformComponent(TransformComponent& transform);
	void DrawInfoSection(Coordinator& coordinator, Entity entity);
	void DrawTransformSection(Coordinator& coordinator, Entity entity);
	void DrawGeometrySection(Coordinator& coordinator, Entity entity);
	void DrawMaterialComponent(MaterialComponent& material);
	void DrawMaterialSection(Coordinator& coordinator, Entity entity);
	void DrawCameraSection(Coordinator& coordinator, Entity entity);
	void DrawSunlightSection(Coordinator& coordinator, Entity entity);
	void DrawComponentSections(Coordinator& coordinator, Entity entity);
	std::string OpenFileDialog_Win32(HWND owner = NULL);
	void DrawRenderPassSettingsWindow(RenderPassSettings* settings);
	HTexture colorCopyTexture; // Texture to copy color data for ImGui rendering
	RenderPassSettings* settings = nullptr;
};
