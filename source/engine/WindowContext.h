#pragma once

#include <Windows.h>
#include "Observer.h"

extern Subject<WinMessageEvent> winMessageSubject;

class WindowContext
{
public:
	WindowContext();
	~WindowContext();
	HWND GetWindowHandle() { return hwnd; }
	LONG GetWidth() { return width; }
	LONG GetHeight() { return height; }
private:
	void RegisterWindowClass();
	void CreateWindowInstance();
	void CreateMenuBar();
	HWND hwnd;
	HWND button;
	HINSTANCE instance;
	LONG width;
	LONG height;
};