#include "ImGuiPass.h"
#include "WindowContext.h"
#include "DeviceContext.h"
#include "RenderContext.h"
#include "DescriptorHeap.h"
#include "Mesh.h"
#include "states/EngineCommandQueue.h"

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h> // For ImGuiDockNodeFlags_DockSpace
#include <filesystem>

extern WindowContext windowContext;
extern DeviceContext deviceContext;
extern RenderContext renderContext;

ImGuiPass::ImGuiPass() : RenderPass(L"ImGui", Type::Drawless)
{
}

void ImGuiPass::ConfigurePipelineState()
{
}

void ImGuiPass::Initialize()
{
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
}

void ImGuiPass::Update()
{
}

void ImGuiPass::Execute()
{
	//renderContext.SetupRenderPass(commandList, pipelineState, rootSignature, viewportAndScissors);
	renderContext.SetDescriptorHeap(commandList);
	renderContext.BindRenderTarget(commandList, HRenderTarget(2));
	// We don't really want to clear, we want to draw on top of the existing content
	//renderContext.CleraRenderTarget(commandList, renderTarget);
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
	
	static int selectedIndex = -1;
	
	ImGui::Text("Game Objects");
	ImGui::Separator();
	
	float panelHeight = ImGui::GetContentRegionAvail().y * 0.5f;
	
	if (ImGui::BeginChild("GameObjectList", ImVec2(0, panelHeight), true))
	{
		for (int i = 0; i < renderContext.GetNumOfMeshes(); ++i)
		{
			auto mesh = renderContext.GetMesh(HMesh(i));
			bool selected = (i == selectedIndex);
			if (ImGui::Selectable(mesh->GetName(), selected))
			{
				selectedIndex = i;
			}
		}
	}
	ImGui::EndChild();
	
	ImGui::Separator();
	ImGui::Text("Details");
	ImGui::Separator();
	
	if (ImGui::BeginChild("DetailsPanel", ImVec2(0, 0), true))
	{
		if (selectedIndex >= 0 && selectedIndex < renderContext.GetNumOfMeshes())
		{
			const auto mesh = renderContext.GetMesh(HMesh(selectedIndex));
			ImGui::Text("Name: %s", mesh->GetName());
			ImGui::Text("Vertices: %d", mesh->GetVertexCount());
		}
		else
		{
			ImGui::Text("No GameObject selected.");
		}
	}
	ImGui::EndChild();
	
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
	// (Your code calls ExecuteCommandLists, swapchain's Present(), etc.)
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
