#include "ImGuiPass.h"
#include "../../engine/WindowContext.h"
#include "../../core/DeviceContext.h"
#include "../RenderContext.h"
#include "../../bind/DescriptorHeap.h"
#include "../../asset/Mesh.h"
#include "../../engine/camera/Camera.h"
#include "../../engine/states/EngineCommandQueue.h"
#include "../../bind/CommandList.h"
#include "../RenderTarget.h"
#include "../../engine/Components.h"
#include "../../engine/Coordinator.h"

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h> // For ImGuiDockNodeFlags_DockSpace
#include <filesystem>
#include <cmath>

extern WindowContext windowContext;
extern DeviceContext deviceContext;
extern RenderContext renderContext;
extern Coordinator* editorCoordinator;

namespace
{
	ImU32 GetHierarchyIconColor(SceneEntityKind kind)
	{
		switch (kind)
		{
		case SceneEntityKind::Camera:
			return IM_COL32(110, 190, 255, 255);
		case SceneEntityKind::Sunlight:
			return IM_COL32(255, 220, 90, 255);
		case SceneEntityKind::Renderable:
		default:
			return IM_COL32(255, 180, 90, 255);
		}
	}

	SunlightConstants ToSunlightConstants(const SunlightComponent& sunlight)
	{
		const float enabled = sunlight.enabled ? 1.0f : 0.0f;
		return SunlightConstants{
			.lightDirection = { sunlight.direction[0], sunlight.direction[1], sunlight.direction[2], 0.0f },
			.lightColor = { sunlight.color[0], sunlight.color[1], sunlight.color[2], 0.0f },
			.ambientStrength = sunlight.ambientStrength * enabled,
			.diffuseStrength = sunlight.diffuseStrength * enabled
		};
	}

	void DrawCameraIcon(ImDrawList* drawList, const ImVec2& topLeft, ImU32 color)
	{
		const ImVec2 bodyMin(topLeft.x + 1.5f, topLeft.y + 4.0f);
		const ImVec2 bodyMax(topLeft.x + 11.5f, topLeft.y + 11.5f);
		const ImVec2 lensCenter(topLeft.x + 6.5f, topLeft.y + 7.8f);
		const ImVec2 viewfinderMin(topLeft.x + 4.0f, topLeft.y + 1.8f);
		const ImVec2 viewfinderMax(topLeft.x + 7.5f, topLeft.y + 4.3f);

		drawList->AddRect(bodyMin, bodyMax, color, 2.5f, 0, 1.8f);
		drawList->AddRectFilled(viewfinderMin, viewfinderMax, color, 1.0f);
		drawList->AddCircle(lensCenter, 2.2f, color, 18, 1.7f);
		drawList->AddCircleFilled(lensCenter, 0.8f, color);

		const ImVec2 barrelA(topLeft.x + 11.5f, topLeft.y + 5.0f);
		const ImVec2 barrelB(topLeft.x + 15.2f, topLeft.y + 3.6f);
		const ImVec2 barrelC(topLeft.x + 15.2f, topLeft.y + 11.9f);
		const ImVec2 barrelD(topLeft.x + 11.5f, topLeft.y + 10.3f);
		drawList->AddQuad(barrelA, barrelB, barrelC, barrelD, color, 1.6f);
	}

	void DrawCubeIcon(ImDrawList* drawList, const ImVec2& topLeft, ImU32 color)
	{
		const ImVec2 frontTL(topLeft.x + 3.0f, topLeft.y + 5.0f);
		const ImVec2 frontTR(topLeft.x + 9.0f, topLeft.y + 5.0f);
		const ImVec2 frontBR(topLeft.x + 9.0f, topLeft.y + 11.0f);
		const ImVec2 frontBL(topLeft.x + 3.0f, topLeft.y + 11.0f);

		const ImVec2 backTL(topLeft.x + 6.0f, topLeft.y + 2.0f);
		const ImVec2 backTR(topLeft.x + 12.0f, topLeft.y + 2.0f);
		const ImVec2 backBR(topLeft.x + 12.0f, topLeft.y + 8.0f);
		const ImVec2 backBL(topLeft.x + 6.0f, topLeft.y + 8.0f);

		drawList->AddQuad(backTL, backTR, backBR, backBL, color, 1.4f);
		drawList->AddQuad(frontTL, frontTR, frontBR, frontBL, color, 1.6f);
		drawList->AddLine(backTL, frontTL, color, 1.4f);
		drawList->AddLine(backTR, frontTR, color, 1.4f);
		drawList->AddLine(backBR, frontBR, color, 1.4f);
		drawList->AddLine(backBL, frontBL, color, 1.4f);
	}

	void DrawSunlightIcon(ImDrawList* drawList, const ImVec2& topLeft, ImU32 color)
	{
		const ImVec2 center(topLeft.x + 8.0f, topLeft.y + 8.0f);
		drawList->AddCircle(center, 3.0f, color, 18, 1.7f);
		for (int i = 0; i < 8; ++i)
		{
			const float angle = (3.14159265f * 2.0f * static_cast<float>(i)) / 8.0f;
			const ImVec2 inner(center.x + cosf(angle) * 5.0f, center.y + sinf(angle) * 5.0f);
			const ImVec2 outer(center.x + cosf(angle) * 7.5f, center.y + sinf(angle) * 7.5f);
			drawList->AddLine(inner, outer, color, 1.4f);
		}
	}
}

ImGuiPass::ImGuiPass() : RenderPass(L"ImGui", L"", Type::Drawless)
{
}

void ImGuiPass::ConfigurePipelineState()
{
}

void ImGuiPass::PostAssetLoad()
{
}

void ImGuiPass::Initialize()
{
	// Create the custom Render Target for ImGui
	renderTarget = renderContext.CreateRenderTarget("RT_ImGui", RenderTargetFormat::RGB8_UNORM);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	const int NUM_FRAMES_IN_FLIGHT = 2; // Number of frames in flight
	ImGui_ImplDX12_InitInfo init_info = {};
	init_info.Device = deviceContext.GetDevice(); // DirectX 12 device
	init_info.CommandQueue = deviceContext.GetCommandQueue(); // DirectX 12 command queue
	init_info.NumFramesInFlight = NUM_FRAMES_IN_FLIGHT;
	init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // Or your render target format.

	// Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
	// The example_win32_directx12/main.cpp application include a simple free-list based allocator.
	init_info.SrvDescriptorHeap = renderContext.GetSrvHeap().GetHeap();
	init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
		{
			renderContext.GetSrvHeap().Allocate(out_cpu_handle, out_gpu_handle); // g_imguiDescriptorHeap points to your DescriptorHeap instance
		};
	init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
		{
			//return YOUR_FREE_FUNCTION_FOR_SRV_DESCRIPTORS(...);
			renderContext.GetSrvHeap().Free(cpu_handle, gpu_handle); // g_imguiDescriptorHeap points to your DescriptorHeap instance
		};

	// (before 1.91.6 the DirectX12 backend required a single SRV descriptor passed)
	// (there is a legacy version of ImGui_ImplDX12_Init() that supports those, but a future version of Dear ImGuii will requires more descriptors to be allocated)
	ImGui_ImplWin32_Init(windowContext.GetWindowHandle()); // Win32 window handle
	ImGui_ImplDX12_Init(&init_info);

	ImFont* font_title = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\CascadiaMono.ttf", 14.0f, NULL, io.Fonts->GetGlyphRangesDefault());

	// Create a texture for the color
	colorCopyTexture = renderContext.CreateEmptyTexture(1920, 1080, DXGI_FORMAT_R8G8B8A8_UNORM, "Color_Copy");
}

void ImGuiPass::Update()
{
}

void ImGuiPass::Execute()
{
	renderContext.SetDescriptorHeap(commandList);
	renderContext.BindRenderTarget(commandList, renderTarget);
	// We don't really want to clear, we want to draw on top of the existing content
	renderContext.CleraRenderTarget(commandList, renderTarget);
	//renderContext.ClearDepthBuffer(commandList, depthBuffer);

	// (Your code process and dispatch Win32 messages)
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow(); // Show demo window! :)

	ImGuiIO& io = ImGui::GetIO();

	float menuHeight = 0.0f;
	if (ImGui::BeginMainMenuBar()) {
		menuHeight = ImGui::GetFrameHeight();
		if (ImGui::BeginMenu("File")) {
			// Open... with shortcut hint
			if (ImGui::MenuItem("Open...", "Ctrl+O") ||
				(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O, false))) {
				std::string path = OpenFileDialog_Win32(windowContext.GetWindowHandle());
				std::filesystem::path fsPath(path);
				std::string filename = fsPath.stem().string();
				GlobalCommandQueue::Push(EngineCommand{ EngineCommandType::UnloadAssets });
				GlobalCommandQueue::Push(EngineCommand{ EngineCommandType::LoadAssets, LoadAssetsPayload{ filename } });
				// Later add Load Assets command with the new scene name
				//GlobalCommandQueue::Push(EngineCommand{ EngineCommandType::GameLoop });
			}

			// Save with shortcut
			if (ImGui::MenuItem("Save", "Ctrl+S") ||
				(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false))) {
				//SaveScene();
			}

			ImGui::Separator();

			// Exit
			if (ImGui::MenuItem("Exit", "Alt+F4"))
			{
				GlobalCommandQueue::Push(EngineCommand{ EngineCommandType::UnloadAssets });
				GlobalCommandQueue::Push(EngineCommand{ EngineCommandType::Exit });
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	ImVec2 window_pos = ImVec2(0, menuHeight);     // Top-left corner
	ImVec2 window_size = ImVec2(400, 1080 - menuHeight - 25);      // 300 px wide, height auto
	
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	
	ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

	// 1. READ DIRECTLY FROM THE SOURCE
// We don't need a static variable. We ask the engine "What is selected right now?"
// This covers both cases: 
// A) We selected something via Viewport in the previous frame (SelectionPass updated Context).
// B) We selected something in ImGui in the previous frame (ImGui updated Context).
	uint32_t currentSelection = renderContext.GetSelectedObjectId();

	ImGui::Text("Game Objects");
	ImGui::Separator();

	float panelHeight = ImGui::GetContentRegionAvail().y * 0.5f;

	if (ImGui::BeginChild("GameObjectList", ImVec2(0, panelHeight), true))
	{
		for (const SceneEntityRecord& entity : renderContext.GetSceneEntities())
		{
			bool isSelected = (currentSelection == entity.id);

			ImGui::PushID(static_cast<int>(entity.id));
			if (ImGui::Selectable("##entity_row", isSelected, ImGuiSelectableFlags_SpanAvailWidth))
			{
				renderContext.SetSelectedObjectId(entity.id);
			}

			ImDrawList* drawList = ImGui::GetWindowDrawList();
			const ImVec2 itemMin = ImGui::GetItemRectMin();
			const ImVec2 itemMax = ImGui::GetItemRectMax();
			const float iconSize = 16.0f;
			const ImVec2 iconTopLeft(itemMin.x + 6.0f, itemMin.y + ((itemMax.y - itemMin.y) - iconSize) * 0.5f);
			const ImU32 iconColor = GetHierarchyIconColor(entity.kind);

			if (entity.kind == SceneEntityKind::Camera)
			{
				DrawCameraIcon(drawList, iconTopLeft, iconColor);
			}
			else if (entity.kind == SceneEntityKind::Sunlight)
			{
				DrawSunlightIcon(drawList, iconTopLeft, iconColor);
			}
			else
			{
				DrawCubeIcon(drawList, iconTopLeft, iconColor);
			}

			const ImVec2 textPos(itemMin.x + 28.0f, itemMin.y + ImGui::GetStyle().FramePadding.y);
			drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), entity.name);
			ImGui::PopID();
		}
	}
	ImGui::EndChild();

	ImGui::Separator();
	ImGui::Text("Details");
	ImGui::Separator();

	if (ImGui::BeginChild("DetailsPanel", ImVec2(0, 0), true))
	{
		// Re-fetch or reuse currentSelection. 
		// Note: If you want immediate feedback within the same frame after a click, 
		// you might want to update 'currentSelection' inside the loop above, 
		// but usually, waiting 1 frame for details to update is imperceptible.
		currentSelection = renderContext.GetSelectedObjectId();

		if (currentSelection != UINT32_MAX)
		{
			SceneEntityRecord* entity = renderContext.GetSceneEntityById(currentSelection);
			if (entity != nullptr)
			{
				ImGui::Text("Name: %s", entity->name);
				ImGui::Text("Entity: %u", entity->id);
				if (entity->kind == SceneEntityKind::Camera)
				{
					Camera* camera = renderContext.GetCamera(entity->cameraIndex);
					const auto position = camera->GetPosition();
					const auto rotation = camera->GetRotation();
					ImGui::Text("Kind: Camera");
					ImGui::Text("Camera Index: %u", entity->cameraIndex);
					ImGui::Text("Projection: %s",
						camera->GetType() == Camera::CameraType::ORTHOGRAPHIC ? "Orthographic" : "Perspective");
					const bool isActiveCamera = renderContext.GetActiveCameraIndex() == entity->cameraIndex;
					ImGui::Text("Active: %s", isActiveCamera ? "Yes" : "No");
					ImGui::Text("Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
					ImGui::Text("Rotation: %.2f, %.2f, %.2f", rotation.x, rotation.y, rotation.z);
					if (!isActiveCamera && ImGui::Button("Make Active Camera"))
					{
						renderContext.SetActiveCamera(entity->cameraIndex);
					}
				}
				else if (entity->kind == SceneEntityKind::Sunlight)
				{
					ImGui::Text("Kind: Sunlight");
					if (editorCoordinator == nullptr)
					{
						ImGui::Text("ECS is unavailable.");
					}
					else
					{
						Signature signature = editorCoordinator->GetEntityManager()->GetSignature(entity->id);
						if (!signature.test(editorCoordinator->GetComponentType<SunlightComponent>()))
						{
							ImGui::Text("Sunlight component missing.");
						}
						else
						{
							SunlightComponent& sunlight = editorCoordinator->GetComponent<SunlightComponent>(entity->id);
							bool changed = false;
							changed |= ImGui::Checkbox("Enabled", &sunlight.enabled);
							changed |= ImGui::DragFloat3("Direction", sunlight.direction, 0.01f, -1.0f, 1.0f);
							changed |= ImGui::ColorEdit3("Color", sunlight.color);
							changed |= ImGui::DragFloat("Ambient", &sunlight.ambientStrength, 0.01f, 0.0f, 1.0f);
							changed |= ImGui::DragFloat("Diffuse", &sunlight.diffuseStrength, 0.01f, 0.0f, 10.0f);

							if (changed)
							{
								renderContext.SetSunlightConstants(ToSunlightConstants(sunlight));
							}
						}
					}
				}
				else
				{
					RenderItem* item = renderContext.GetRenderItemById(currentSelection);
					if (item != nullptr)
					{
						const auto mesh = renderContext.GetMesh(item->mesh);
						ImGui::Text("Kind: Renderable");
						ImGui::Text("Vertices: %d", mesh->GetVertexCount());
						ImGui::Text("Mesh Handle: %zu", item->mesh.Index());
						ImGui::Text("Texture Handle: %zu", item->texture.Index());
					}
				}
			}
			else
			{
				ImGui::Text("No GameObject selected.");
			}
		}
		else
		{
			ImGui::Text("No GameObject selected.");
		}
	}
	ImGui::EndChild();

	ImGui::End();

	ImVec2 viewport_pos = ImVec2(400, menuHeight);     // Top-left corner
	ImVec2 viewport_size = ImVec2(1920 - 400, 1080 - menuHeight - 25);      // 300 px wide, height auto

	ImGui::SetNextWindowPos(viewport_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(viewport_size, ImGuiCond_Always);
	ImGui::Begin("Viewport");

	HTexture finalSceneTexture = renderContext.GetTexture("CompositionTexture");
	auto finalScene = renderContext.GetTexture(finalSceneTexture);
	renderContext.TransitionTo(commandList, finalSceneTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	auto srvHandleGPU = renderContext.GetSrvHeap().GetGPU(DescriptorHeap::HeapPartition::STATIC, finalScene->GetSrvDescriptorIndex());
	ImTextureID textureID = (ImTextureID)srvHandleGPU.ptr;
	ImVec2 size = ImGui::GetContentRegionAvail();
	ImGui::Image(textureID, size);
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 25));
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 25));
	ImGui::Begin("StatusBar", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings);

	float currentMB, budgetMB, usagePct;
	deviceContext.GetMemoryUsage(currentMB, budgetMB, usagePct);

	std::stringstream ss;
	ss << "GPU VRAM: " << std::fixed << std::setprecision(1)
		<< currentMB << " MB / " << budgetMB << " MB (" << std::setprecision(0) << usagePct << "%)";
	std::string vramLabel = ss.str();


	// Color the bar based on usage
	if (usagePct > 95.0f) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
	else if (usagePct > 80.0f) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 165, 0, 255));
	else ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));

	ImGui::Text("%s", vramLabel.c_str());
	ImGui::PopStyleColor();

	ImGui::End();


	// Rendering
	// (Your code clears your framebuffer, renders your other stuff etc.)
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), renderContext.GetCommandList(commandList)->GetCommandList());
	renderContext.TransitionBack(commandList, finalSceneTexture);
}

void ImGuiPass::PostSubmit()
{
}

void ImGuiPass::Allocate(DeviceContext* deviceContext)
{
}

std::string ImGuiPass::OpenFileDialog_Win32(HWND owner)
{
	char filename[MAX_PATH] = { 0 };
	OPENFILENAMEA ofn = {};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = owner;  // set your window's HWND
	ofn.lpstrFilter = "XML Scene Files\0*.xml\0All Files\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn)) {
		return std::string(filename);
	}
	return "";
}
