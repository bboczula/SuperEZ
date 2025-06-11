#include <list>

#include "WindowContext.h"
#include "Utils.h"
#include "CommCtrl.h"

#define WINDOW_CLASS_NAME L"SuperEZMyWindowWin32Class"
#define WINDOW_TITLE L"SuperEZ Engine v1.3"
#define DEFAULT_WINDOW_STYLE WS_VISIBLE | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER

#define ID_FILE_ABOUT 1
#define ID_FILE_EXIT  2

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
}

WindowContext::WindowContext() : width(1920), height(1080), instance(GetModuleHandle(nullptr))
{
	OutputDebugString(L"WindowContext Constructor\n");
	RegisterWindowClass();
	CreateWindowInstance();
}

WindowContext::~WindowContext()
{
	OutputDebugString(L"WindowContext Destructor\n");
}

void WindowContext::RegisterWindowClass()
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
}

void WindowContext::CreateWindowInstance()
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
