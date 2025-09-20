#include <list>

#include "../Utils.h"
#include "CommCtrl.h"

// WindowContext.cpp  (Windows-only guts live here)
#include "WindowContext.h"

// Centralize Windows.h to one place
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#define WINDOW_CLASS_NAME L"SuperEZMyWindowWin32Class"
#define WINDOW_TITLE L"SuperEZ Engine v1.4"
#define DEFAULT_WINDOW_STYLE WS_VISIBLE | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER

#define ID_FILE_ABOUT 1
#define ID_FILE_EXIT  2

struct WindowContext::Impl
{
	// Private implementation details would go here
	HWND hwnd {};
	HINSTANCE instance {};
	int width {};
	int height {};

	static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WinMessageEvent winMessageEvent(hwnd, uMsg, wParam, lParam);
		winMessageSubject.Notify(winMessageEvent);
		switch (uMsg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		return 0;
	};

	void RegisterWindowClass()
	{
		OutputDebugString(L"RegisterWindowClass\n");

		WNDCLASSEX windowClass;
		ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.lpszClassName = WINDOW_CLASS_NAME;
		windowClass.lpfnWndProc = WindowProcedure;
		windowClass.hInstance = instance;

		ExitIfFailed(RegisterClassEx(&windowClass));
		OutputDebugString(L"Successfully registered window class\n");
	};

	void CreateWindowInstance()
	{
		OutputDebugString(L"CreateWindowInstance\n");

		std::pair<LONG, LONG> windowDimentions;

		RECT windowRectangle = { 0, 0, width, height };
		AdjustWindowRect(&windowRectangle, DEFAULT_WINDOW_STYLE, FALSE);
		windowDimentions.first = windowRectangle.right - windowRectangle.left;
		windowDimentions.second = windowRectangle.bottom - windowRectangle.top;

		hwnd = CreateWindowEx(NULL, WINDOW_CLASS_NAME, WINDOW_TITLE, DEFAULT_WINDOW_STYLE,
			CW_USEDEFAULT, CW_USEDEFAULT, windowDimentions.first, windowDimentions.second, nullptr, nullptr, instance, this);

		if (!hwnd)
		{
			OutputDebugString(L"ERROR: Window creation failed\n");
			exit(1);
		}
		OutputDebugString(L"Successfully created window\n");
	}
};

WindowContext::WindowContext()
{
	OutputDebugString(L"WindowContext Constructor\n");
	pImpl = new Impl{};
	pImpl->width = 1920;
	pImpl->height = 1080;
	pImpl->instance = GetModuleHandle(nullptr);
	pImpl->RegisterWindowClass();
	pImpl->CreateWindowInstance();
}

WindowContext::~WindowContext() = default;

HWND WindowContext::GetWindowHandle()
{
	return pImpl->hwnd;
}

long WindowContext::GetWidth()
{
	return pImpl->width;
}

long WindowContext::GetHeight()
{
	return pImpl->height;
}
