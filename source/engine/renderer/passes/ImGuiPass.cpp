#include "ImGuiPass.h"
#include "../../engine/WindowContext.h"
#include "../../core/DeviceContext.h"
#include "../RenderContext.h"
#include "../../bind/DescriptorHeap.h"
#include "../../asset/Mesh.h"
#include "../../engine/camera/Camera.h"
#include "../../engine/Engine.h"
#include "../../engine/states/EngineCommandQueue.h"
#include "../../bind/CommandList.h"
#include "../RenderTarget.h"

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h> // For ImGuiDockNodeFlags_DockSpace
#include <filesystem>
#include <fstream>
#include <cmath>

extern WindowContext windowContext;
extern DeviceContext deviceContext;
extern RenderContext renderContext;
extern Coordinator* editorCoordinator;
extern Engine* editorEngine;

namespace
{
	template<typename T>
	bool HasComponent(Coordinator& coordinator, Entity entity)
	{
		Signature signature = coordinator.GetEntityManager()->GetSignature(entity);
		return signature.test(coordinator.GetComponentType<T>());
	}

	SunlightConstants ToSunlightConstants(const SunlightComponent& sunlight)
	{
		const float enabled = sunlight.enabled ? 1.0f : 0.0f;
		return SunlightConstants{
			.lightDirection = { sunlight.direction[0], sunlight.direction[1], sunlight.direction[2], 0.0f },
			.lightColor = { sunlight.color[0], sunlight.color[1], sunlight.color[2], 0.0f },
			.ambientStrength = sunlight.ambientStrength * enabled,
			.diffuseStrength = sunlight.diffuseStrength * enabled,
			.shadowBias = sunlight.shadowBias,
			.shadowSlopeBias = sunlight.shadowSlopeBias
		};
	}
}

ImGuiPass::ImGuiPass(RenderPassSettings* settings) : RenderPass(L"ImGui", L"", Type::Drawless), settings(settings)
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
	TextureCreateDesc textureDesc;
	textureDesc.width = 1920;
	textureDesc.height = 1080;
	textureDesc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.name = "Color_Copy";
	colorCopyTexture = renderContext.CreateEmptyTexture(textureDesc);
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

			// Save with shortcut - overwrites the currently loaded scene file,
			// or the last "Save As..." target if one was chosen this session.
			if (ImGui::MenuItem("Save", "Ctrl+S") ||
				(io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false))) {
				std::filesystem::path targetPath = saveAsPath;
				if (targetPath.empty() && editorEngine != nullptr)
				{
					const std::string sceneName = editorEngine->GetCurrentSceneName();
					if (!sceneName.empty())
					{
						targetPath = std::filesystem::current_path() / "assets" / sceneName / (sceneName + ".xml");
					}
				}
				if (!targetPath.empty())
				{
					SaveSceneToXml(targetPath);
				}
			}

			if (ImGui::MenuItem("Save As...")) {
				const std::string path = SaveFileDialog_Win32(windowContext.GetWindowHandle());
				if (!path.empty())
				{
					saveAsPath = path;
					SaveSceneToXml(saveAsPath);
				}
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

		if (ImGui::BeginMenu("Debug")) {
			if (ImGui::MenuItem("Render Pass Settings")) {
				ImGui::SetWindowFocus("Render Pass Settings");
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
		if (editorCoordinator == nullptr)
		{
			ImGui::Text("ECS is unavailable.");
		}
		else
		{
			for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
			{
				if (!HasComponent<InfoComponent>(*editorCoordinator, entity))
				{
					continue;
				}

				InfoComponent& info = editorCoordinator->GetComponent<InfoComponent>(entity);
				const bool isSelected = (currentSelection == entity);

				ImGui::PushID(static_cast<int>(entity));
				if (ImGui::Selectable(info.name.c_str(), isSelected, ImGuiSelectableFlags_SpanAvailWidth))
				{
					renderContext.SetSelectedObjectId(entity);
				}
				ImGui::PopID();
			}
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
			if (editorCoordinator == nullptr)
			{
				ImGui::Text("ECS is unavailable.");
			}
			else if (HasComponent<InfoComponent>(*editorCoordinator, currentSelection))
			{
				ImGui::Text("Entity: %u", currentSelection);
				DrawComponentSections(*editorCoordinator, currentSelection);
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

	// When the debug RT viewer is active (Render Pass Settings -> Blit -> Source),
	// show the visualized texture in the viewport instead of the scene.
	HTexture finalSceneTexture = renderContext.IsDebugViewActive()
		? renderContext.GetTexture("DebugBlitTexture")
		: renderContext.GetTexture("CompositionTexture");
	auto finalScene = renderContext.GetTexture(finalSceneTexture);
	renderContext.TransitionTo(commandList, finalSceneTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	auto srvHandleGPU = renderContext.GetSrvHeap().GetGPU(DescriptorHeap::HeapPartition::STATIC, finalScene->GetSrvDescriptorIndex());
	ImTextureID textureID = (ImTextureID)srvHandleGPU.ptr;
	ImVec2 size = ImGui::GetContentRegionAvail();
	if (renderContext.IsDebugViewActive())
	{
		// DebugBlitTexture is 16:9; stretching it to the viewport region would
		// distort it, so fit it by aspect ratio and center it instead.
		const float textureAspect = 1920.0f / 1080.0f;
		ImVec2 fitted = size;
		if (fitted.x / fitted.y > textureAspect)
		{
			fitted.x = fitted.y * textureAspect;
		}
		else
		{
			fitted.y = fitted.x / textureAspect;
		}

		ImVec2 cursor = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(
			cursor.x + (size.x - fitted.x) * 0.5f,
			cursor.y + (size.y - fitted.y) * 0.5f));
		ImGui::Image(textureID, fitted);
	}
	else
	{
		ImGui::Image(textureID, size);
	}
	ImGui::End();

	if (settings != nullptr)
	{
		DrawRenderPassSettingsWindow(settings);
	}

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

void ImGuiPass::DrawInfoComponent(InfoComponent& info)
{
	ImGui::Text("Name: %s", info.name.c_str());
}

void ImGuiPass::DrawInfoSection(Coordinator& coordinator, Entity entity)
{
	InfoComponent& info = coordinator.GetComponent<InfoComponent>(entity);
	DrawInfoComponent(info);
}

void ImGuiPass::DrawTransformComponent(TransformComponent& transform)
{
	ImGui::DragFloat3("Position", transform.position, 0.01f);

	// Rotation is stored in radians, but degrees are easier to reason about in the UI.
	float rotationDegrees[3] = {
		DirectX::XMConvertToDegrees(transform.rotation[0]),
		DirectX::XMConvertToDegrees(transform.rotation[1]),
		DirectX::XMConvertToDegrees(transform.rotation[2])
	};
	if (ImGui::DragFloat3("Rotation", rotationDegrees, 1.0f))
	{
		transform.rotation[0] = DirectX::XMConvertToRadians(rotationDegrees[0]);
		transform.rotation[1] = DirectX::XMConvertToRadians(rotationDegrees[1]);
		transform.rotation[2] = DirectX::XMConvertToRadians(rotationDegrees[2]);
	}

	ImGui::DragFloat3("Scale", transform.scale, 0.01f, 0.0001f, 100.0f);
}

void ImGuiPass::DrawTransformSection(Coordinator& coordinator, Entity entity)
{
	TransformComponent& transform = coordinator.GetComponent<TransformComponent>(entity);
	DrawTransformComponent(transform);
}

void ImGuiPass::DrawGeometrySection(Coordinator& coordinator, Entity entity)
{
	GeometryComponent& geometry = coordinator.GetComponent<GeometryComponent>(entity);

	ImGui::Text("Mesh Handle: %zu", geometry.meshHandle.Index());
	if (geometry.meshHandle.IsValid())
	{
		const auto mesh = renderContext.GetMesh(geometry.meshHandle);
		ImGui::Text("Vertices: %d", mesh->GetVertexCount());
	}
}

void ImGuiPass::DrawMaterialComponent(MaterialComponent& material)
{
	ImGui::Text("Texture Handle: %zu", material.textureHandle.Index());
	ImGui::DragFloat("Diffuse Strength", &material.diffuseStrength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Specular Strength", &material.specularStrength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Shininess", &material.shininess, 0.1f, 1.0f, 256.0f);
}

void ImGuiPass::DrawMaterialSection(Coordinator& coordinator, Entity entity)
{
	MaterialComponent& material = coordinator.GetComponent<MaterialComponent>(entity);
	DrawMaterialComponent(material);
}

void ImGuiPass::DrawCameraSection(Coordinator& coordinator, Entity entity)
{
	CameraComponent& cameraComponent = coordinator.GetComponent<CameraComponent>(entity);
	Camera* camera = renderContext.GetCamera(static_cast<UINT>(cameraComponent.cameraIndex));
	const auto position = camera->GetPosition();
	const auto rotation = camera->GetRotation();
	const bool isActiveCamera = renderContext.GetActiveCameraIndex() == cameraComponent.cameraIndex;

	ImGui::Text("Camera Index: %zu", cameraComponent.cameraIndex);
	ImGui::Text("Projection: %s",
		camera->GetType() == Camera::CameraType::ORTHOGRAPHIC ? "Orthographic" : "Perspective");
	ImGui::Text("Active: %s", isActiveCamera ? "Yes" : "No");
	ImGui::Text("Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ImGui::Text("Rotation: %.2f, %.2f, %.2f", rotation.x, rotation.y, rotation.z);

	if (!isActiveCamera && ImGui::Button("Make Active Camera"))
	{
		renderContext.SetActiveCamera(static_cast<UINT>(cameraComponent.cameraIndex));
		for (Entity candidate = 0; candidate < MAX_ENTITIES; ++candidate)
		{
			if (HasComponent<CameraComponent>(coordinator, candidate))
			{
				coordinator.GetComponent<CameraComponent>(candidate).active = (candidate == entity);
			}
		}
	}
}

void ImGuiPass::DrawSunlightSection(Coordinator& coordinator, Entity entity)
{
	SunlightComponent& sunlight = coordinator.GetComponent<SunlightComponent>(entity);
	bool changed = false;
	changed |= ImGui::Checkbox("Enabled", &sunlight.enabled);
	changed |= ImGui::DragFloat3("Direction", sunlight.direction, 0.01f, -1.0f, 1.0f);
	changed |= ImGui::ColorEdit3("Color", sunlight.color);
	changed |= ImGui::DragFloat("Ambient", &sunlight.ambientStrength, 0.01f, 0.0f, 1.0f);
	changed |= ImGui::DragFloat("Diffuse", &sunlight.diffuseStrength, 0.01f, 0.0f, 10.0f);
	changed |= ImGui::DragFloat("Shadow Bias", &sunlight.shadowBias, 0.0001f, 0.0f, 0.05f, "%.6f");
	changed |= ImGui::DragFloat("Shadow Slope Bias", &sunlight.shadowSlopeBias, 0.0001f, 0.0f, 0.05f, "%.6f");

	if (changed)
	{
		renderContext.SetSunlightConstants(ToSunlightConstants(sunlight));
	}
}

void ImGuiPass::DrawComponentSections(Coordinator& coordinator, Entity entity)
{
	struct ComponentDrawer
	{
		const char* label;
		bool (*has)(Coordinator&, Entity);
		void (ImGuiPass::*draw)(Coordinator&, Entity);
	};

	static const ComponentDrawer drawers[] =
	{
		{ "Info", HasComponent<InfoComponent>, &ImGuiPass::DrawInfoSection },
		{ "Transform", HasComponent<TransformComponent>, &ImGuiPass::DrawTransformSection },
		{ "Geometry", HasComponent<GeometryComponent>, &ImGuiPass::DrawGeometrySection },
		{ "Material", HasComponent<MaterialComponent>, &ImGuiPass::DrawMaterialSection },
		{ "Camera", HasComponent<CameraComponent>, &ImGuiPass::DrawCameraSection },
		{ "Sunlight", HasComponent<SunlightComponent>, &ImGuiPass::DrawSunlightSection },
	};

	for (const ComponentDrawer& drawer : drawers)
	{
		if (!drawer.has(coordinator, entity))
		{
			continue;
		}

		if (ImGui::CollapsingHeader(drawer.label, ImGuiTreeNodeFlags_DefaultOpen))
		{
			(this->*drawer.draw)(coordinator, entity);
		}
	}
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

std::string ImGuiPass::SaveFileDialog_Win32(HWND owner)
{
	char filename[MAX_PATH] = { 0 };

	// Default to the current scene's own folder so relative mesh/texture
	// references in the saved XML still resolve correctly.
	std::string initialDir;
	if (editorEngine != nullptr)
	{
		const std::string sceneName = editorEngine->GetCurrentSceneName();
		if (!sceneName.empty())
		{
			initialDir = (std::filesystem::current_path() / "assets" / sceneName).string();
			strncpy_s(filename, (sceneName + ".xml").c_str(), _TRUNCATE);
		}
	}

	OPENFILENAMEA ofn = {};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = "XML Scene Files\0*.xml\0All Files\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "xml";
	ofn.lpstrInitialDir = initialDir.empty() ? nullptr : initialDir.c_str();
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

	if (GetSaveFileNameA(&ofn)) {
		return std::string(filename);
	}
	return "";
}

namespace
{
	void WritePositionElement(tinyxml2::XMLPrinter& printer, const float (&values)[3])
	{
		printer.OpenElement("Position");
		printer.PushAttribute("x", values[0]);
		printer.PushAttribute("y", values[1]);
		printer.PushAttribute("z", values[2]);
		printer.CloseElement();
	}

	void WriteScaleElement(tinyxml2::XMLPrinter& printer, const float (&values)[3])
	{
		printer.OpenElement("Scale");
		printer.PushAttribute("x", values[0]);
		printer.PushAttribute("y", values[1]);
		printer.PushAttribute("z", values[2]);
		printer.CloseElement();
	}

	void WriteObjectRotationElement(tinyxml2::XMLPrinter& printer, const float (&values)[3])
	{
		printer.OpenElement("Rotation");
		printer.PushAttribute("x", values[0]);
		printer.PushAttribute("y", values[1]);
		printer.PushAttribute("z", values[2]);
		printer.CloseElement();
	}

	void WriteCameraRotationElement(tinyxml2::XMLPrinter& printer, const DirectX::SimpleMath::Vector3& rotation)
	{
		printer.OpenElement("Rotation");
		printer.PushAttribute("pitch", rotation.x);
		printer.PushAttribute("yaw", rotation.y);
		printer.PushAttribute("roll", rotation.z);
		printer.CloseElement();
	}
}

void ImGuiPass::SaveSceneToXml(const std::filesystem::path& path)
{
	if (editorCoordinator == nullptr)
	{
		return;
	}

	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Scene");

	for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
	{
		if (!HasComponent<CameraComponent>(*editorCoordinator, entity) || !HasComponent<InfoComponent>(*editorCoordinator, entity))
		{
			continue;
		}

		InfoComponent& info = editorCoordinator->GetComponent<InfoComponent>(entity);
		CameraComponent& cameraComponent = editorCoordinator->GetComponent<CameraComponent>(entity);
		Camera* camera = renderContext.GetCamera(static_cast<UINT>(cameraComponent.cameraIndex));

		printer.OpenElement("Camera");
		printer.PushAttribute("name", info.name.c_str());
		const auto position = camera->GetPosition();
		printer.OpenElement("Position");
		printer.PushAttribute("x", position.x);
		printer.PushAttribute("y", position.y);
		printer.PushAttribute("z", position.z);
		printer.CloseElement();
		WriteCameraRotationElement(printer, camera->GetRotation());
		printer.CloseElement(); // Camera
	}

	for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
	{
		if (!HasComponent<SunlightComponent>(*editorCoordinator, entity) || !HasComponent<InfoComponent>(*editorCoordinator, entity))
		{
			continue;
		}

		InfoComponent& info = editorCoordinator->GetComponent<InfoComponent>(entity);
		SunlightComponent& sunlight = editorCoordinator->GetComponent<SunlightComponent>(entity);

		printer.OpenElement("Sunlight");
		printer.PushAttribute("name", info.name.c_str());
		printer.PushAttribute("enabled", sunlight.enabled);
		printer.OpenElement("Direction");
		printer.PushAttribute("x", sunlight.direction[0]);
		printer.PushAttribute("y", sunlight.direction[1]);
		printer.PushAttribute("z", sunlight.direction[2]);
		printer.CloseElement();
		printer.OpenElement("Color");
		printer.PushAttribute("r", sunlight.color[0]);
		printer.PushAttribute("g", sunlight.color[1]);
		printer.PushAttribute("b", sunlight.color[2]);
		printer.CloseElement();
		printer.OpenElement("Lighting");
		printer.PushAttribute("ambient", sunlight.ambientStrength);
		printer.PushAttribute("diffuse", sunlight.diffuseStrength);
		printer.PushAttribute("shadowBias", sunlight.shadowBias);
		printer.PushAttribute("shadowSlopeBias", sunlight.shadowSlopeBias);
		printer.CloseElement();
		printer.CloseElement(); // Sunlight
	}

	const std::string meshLibraryFile = editorEngine != nullptr ? editorEngine->GetCurrentMeshLibraryFile() : "";
	printer.OpenElement("MeshLibrary");
	printer.PushAttribute("file", meshLibraryFile.c_str());
	printer.CloseElement();

	for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
	{
		if (!HasComponent<TransformComponent>(*editorCoordinator, entity) ||
			!HasComponent<GeometryComponent>(*editorCoordinator, entity) ||
			!HasComponent<MaterialComponent>(*editorCoordinator, entity) ||
			!HasComponent<InfoComponent>(*editorCoordinator, entity))
		{
			continue;
		}

		InfoComponent& info = editorCoordinator->GetComponent<InfoComponent>(entity);
		TransformComponent& transform = editorCoordinator->GetComponent<TransformComponent>(entity);
		GeometryComponent& geometry = editorCoordinator->GetComponent<GeometryComponent>(entity);
		MaterialComponent& material = editorCoordinator->GetComponent<MaterialComponent>(entity);
		Mesh* mesh = renderContext.GetMesh(geometry.meshHandle);

		printer.OpenElement("GameObject");
		printer.PushAttribute("name", info.name.c_str());
		printer.PushAttribute("mesh", mesh->GetName());
		printer.PushAttribute("texture", material.textureFileName.c_str());
		WritePositionElement(printer, transform.position);
		WriteObjectRotationElement(printer, transform.rotation);
		WriteScaleElement(printer, transform.scale);
		printer.CloseElement(); // GameObject
	}

	printer.CloseElement(); // Scene

	std::error_code errorCode;
	std::filesystem::create_directories(path.parent_path(), errorCode);

	std::ofstream out(path, std::ios::binary);
	out << printer.CStr();
}

void ImGuiPass::DrawRenderPassSettingsWindow(RenderPassSettings* settings)
{
	ImGui::Begin("Render Pass Settings");

	if (ImGui::BeginTabBar("RenderPassSettingsTabs"))
	{
		for (const RenderPassSettingsGroup& group : settings->GetGroups())
		{
			char passName[128] = {};
			wcstombs_s(nullptr, passName, group.passName, _TRUNCATE);

			if (ImGui::BeginTabItem(passName))
			{
				for (const RenderPassSetting& setting : group.settings)
				{
					bool changed = false;

					if (setting.type == RenderPassSettingType::Bool)
					{
						changed = ImGui::Checkbox(setting.label, static_cast<bool*>(setting.value));
					}
					else if (setting.type == RenderPassSettingType::Float)
					{
						changed = ImGui::DragFloat(setting.label, static_cast<float*>(setting.value), setting.step, setting.min, setting.max);
					}
					else if (setting.type == RenderPassSettingType::Combo)
					{
						changed = ImGui::Combo(
							setting.label,
							static_cast<int*>(setting.value),
							setting.comboItems,
							setting.comboItemCount);
					}
					else if (setting.type == RenderPassSettingType::Text)
					{
						ImGui::Spacing();
						ImGui::SeparatorText(setting.label);
						ImGui::TextUnformatted(static_cast<const char*>(setting.value));
					}
					else if (setting.type == RenderPassSettingType::ColorLegend)
					{
						ImGui::TextUnformatted(setting.label);
						if (ImGui::BeginTable(setting.name, 4))
						{
							for (int i = 0; i < setting.legendItemCount; ++i)
							{
								const unsigned int color = setting.legendColors[i];
								const ImVec4 colorValue(
									static_cast<float>((color >> 24) & 0xff) / 255.0f,
									static_cast<float>((color >> 16) & 0xff) / 255.0f,
									static_cast<float>((color >> 8) & 0xff) / 255.0f,
									static_cast<float>(color & 0xff) / 255.0f);

								ImGui::TableNextColumn();
								ImGui::PushID(i);
								ImGui::ColorButton("##color", colorValue, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(14.0f, 14.0f));
								ImGui::SameLine();
								ImGui::TextUnformatted(setting.legendLabels[i]);
								ImGui::PopID();
							}
							ImGui::EndTable();
						}
					}

					if (changed && setting.onChanged)
					{
						setting.onChanged(setting.userData);
					}
				}
				ImGui::EndTabItem();

			}
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}
