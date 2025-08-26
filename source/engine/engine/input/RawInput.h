#pragma once

#include "../Observer.h"
#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <hidusage.h>
#include <vector>

#define MAX_NUM_OF_KEYS 254
#define MAX_NUM_OF_MOUSE_BUTTONS 4

enum class MouseButton : size_t
{
	Left = 0,
	Middle = 1,
	Right = 2,
	Wheel = 3
};

class RawInput : public IObserver<WinMessageEvent>
{
public:
	void OnNotify(WinMessageEvent& event) override;
	void Initialize() override;
	void PostFrame() override;
	long GetMouseXDelta();
	long GetMouseYDelta();
	long GetMouseXPosition() { return mousePosition.first; }
	long GetMouseYPosition() { return mousePosition.second; }
	long GetMouseWheelDelta() { return wheelDelta; }
	bool IsLeftButtonDown() { return mouseButtonState[0]; }
	bool IsMiddleButtonDown() { return mouseButtonState[1]; }
	bool IsRightButtonDown() { return mouseButtonState[2]; }
	bool HasMouseWheelMoved() { return mouseButtonState[3]; }
	bool IsKeyDown(WPARAM virtualKeyCode);
	bool WasKeyDown(WPARAM virtualKeyCode);
private:
	void HandleKeyboardInput(const RAWINPUT& rawInput);
	void HandleMouseInput(const RAWINPUT& rawInput);
	void HandleMouseWheel(const USHORT flags, const USHORT data);
	void HandleMousePosition(const RAWMOUSE& mouseData);
	void HandleMouseButtonStates(const USHORT flags);
	BOOL virtualKeyState[MAX_NUM_OF_KEYS];
	BOOL prevVirtualKeyState[MAX_NUM_OF_KEYS];
	std::pair<LONG, LONG> mouseDelta;
	std::pair<LONG, LONG> mousePosition;
	std::vector<BYTE> rawBuffer;
	std::pair<BOOL, USHORT> clearNextFrame;
	BOOL mouseButtonState[MAX_NUM_OF_MOUSE_BUTTONS];
	SHORT wheelDelta;
};