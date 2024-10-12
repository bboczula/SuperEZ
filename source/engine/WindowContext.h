#pragma once

#include <Windows.h>

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
	HINSTANCE instance;
	LONG width;
	LONG height;
};