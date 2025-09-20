#pragma once

#include <cstdint>

// Forward-declare Windows handle type so headers don't drag <Windows.h>
struct HWND__;
using HWND = HWND__*;

// A lightweight POD replacement for MSG
// (mirrors HWND, UINT, WPARAM, LPARAM)
struct WinMessageEvent
{
	HWND hwnd;               // native window handle (forward-declared only)
	std::uint32_t msg;       // message code (was UINT)
	std::uintptr_t wParam;   // unsigned, pointer-sized (was WPARAM)
	std::intptr_t  lParam;   // signed, pointer-sized (was LPARAM)
};