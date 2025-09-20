#pragma once

#include "Observer.h"

// Forward-declare Windows handle types so we can expose them without including <Windows.h>
struct HWND__;      using HWND = HWND__*;
struct HINSTANCE__; using HINSTANCE = HINSTANCE__*;

extern Subject<WinMessageEvent> winMessageSubject;

class WindowContext
{
public:
	WindowContext();
	~WindowContext();
	HWND GetWindowHandle();
	long GetWidth();
	long GetHeight();
private:
	struct Impl;
	Impl* pImpl;
};