#include <list>

#include "WindowContext.h"
#include "Utils.h"
#include "CommCtrl.h"

#define WINDOW_CLASS_NAME L"SapphireMyWindowWin32Class"
#define WINDOW_TITLE L"SapphireEngine"
#define DEFAULT_WINDOW_STYLE WS_VISIBLE | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER

#define ID_FILE_ABOUT 1
#define ID_FILE_EXIT  2

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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

WindowContext::WindowContext() : width(800), height(600), instance(GetModuleHandle(nullptr))
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

	//button = CreateWindowEx(0,
	//	WC_TREEVIEW,
	//	TEXT("Tree View"),
	//	WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES,
	//	0,
	//	0,
	//	windowDimentions.first * 0.25f,
	//	windowDimentions.second,
	//	hwnd,
	//	nullptr,
	//	instance,
	//	NULL);

	CreateMenuBar();
}

void WindowContext::CreateMenuBar()
{
	HMENU hMenubar = CreateMenu();
	HMENU hMenu = CreateMenu();

	AppendMenu(hMenu, MF_STRING, ID_FILE_ABOUT, L"About");
	AppendMenu(hMenu, MF_STRING, ID_FILE_EXIT, L"Exit");
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"File");

	SetMenu(hwnd, hMenubar);
}
