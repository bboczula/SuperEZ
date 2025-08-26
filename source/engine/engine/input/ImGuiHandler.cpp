#include "ImGuiHandler.h"

#include <imgui_impl_win32.h>

#include "../WindowContext.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern WindowContext windowContext;

void ImGuiHandler::OnNotify(WinMessageEvent& event)
{
	ImGui_ImplWin32_WndProcHandler(windowContext.GetWindowHandle(), event.msg, event.wParam, event.lParam);
}

void ImGuiHandler::Initialize()
{
}

void ImGuiHandler::PostFrame()
{
}
