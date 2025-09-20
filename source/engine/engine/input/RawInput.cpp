#include "RawInput.h"

void RawInput::OnNotify(WinMessageEvent& event)
{
	switch (event.msg)
	{
	case WM_INPUT:
	{
		UINT dwSize = { 0 };
		GetRawInputData((HRAWINPUT)event.lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

		rawBuffer.resize(dwSize);
		GetRawInputData((HRAWINPUT)event.lParam, RID_INPUT, rawBuffer.data(), &dwSize, sizeof(RAWINPUTHEADER));

		auto& rawInput = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
		if (rawInput.header.dwType == RIM_TYPEKEYBOARD)
		{
			HandleKeyboardInput(rawInput);
		}
		else if (rawInput.header.dwType == RIM_TYPEMOUSE)
		{
			HandleMouseInput(rawInput);
		}
	}
	}
}

void RawInput::HandleKeyboardInput(const RAWINPUT& rawInput)
{
      auto virtualKey = rawInput.data.keyboard.VKey;
      prevVirtualKeyState[virtualKey] = virtualKeyState[virtualKey];
      if (rawInput.data.keyboard.Flags == 0)
      {
            virtualKeyState[virtualKey] = true;
      }
      else
      {
            virtualKeyState[virtualKey] = false;
            clearNextFrame = { true, virtualKey };
      }
}

void RawInput::HandleMouseInput(const RAWINPUT& rawInput)
{
	const RAWMOUSE& mouseData = rawInput.data.mouse;
	const USHORT flags = rawInput.data.mouse.usButtonFlags;
	const USHORT data = rawInput.data.mouse.usButtonData;

	HandleMousePosition(mouseData);
	HandleMouseButtonStates(flags);
	HandleMouseWheel(flags, data);
}

void RawInput::HandleMouseWheel(const USHORT flags, const USHORT data)
{
	if (flags & RI_MOUSE_WHEEL)
	{
		mouseButtonState[static_cast<size_t>(MouseButton::Wheel)] = true;
		wheelDelta = data;
	}
}

void RawInput::HandleMousePosition(const RAWMOUSE& mouseData)
{
	mousePosition.first = mouseData.lLastX;
	mousePosition.second = mouseData.lLastY;

	mouseDelta.first += mouseData.lLastX;
	mouseDelta.second += mouseData.lLastY;
}

void RawInput::HandleMouseButtonStates(const USHORT flags)
{
	struct ButtonMap
	{
		USHORT downFlag;
		USHORT upFlag;
		MouseButton index;
	};

	const ButtonMap buttonMap[] =
	{
		{ RI_MOUSE_LEFT_BUTTON_DOWN,   RI_MOUSE_LEFT_BUTTON_UP,   MouseButton::Left },
		{ RI_MOUSE_MIDDLE_BUTTON_DOWN, RI_MOUSE_MIDDLE_BUTTON_UP, MouseButton::Middle },
		{ RI_MOUSE_RIGHT_BUTTON_DOWN,  RI_MOUSE_RIGHT_BUTTON_UP,  MouseButton::Right }
	};

	for (const auto& btn : buttonMap)
	{
		if (flags & btn.downFlag)
		{
			mouseButtonState[static_cast<size_t>(btn.index)] = true;
		}
		if (flags & btn.upFlag)
		{
			mouseButtonState[static_cast<size_t>(btn.index)] = false;
		}
	}
}

void RawInput::Initialize()
{
	RAWINPUTDEVICE devices[2];

	devices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	devices[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	devices[0].dwFlags = 0;
	devices[0].hwndTarget = 0;
	
	devices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	devices[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	devices[1].dwFlags = 0;
	devices[1].hwndTarget = 0;
	
	if (RegisterRawInputDevices(devices, 2, sizeof(RAWINPUTDEVICE)) == FALSE)
	{
		OutputDebugString(L"ERROR: RawInput devices registration failes!\n");
	}
	else
	{
		OutputDebugString(L"RawInput devices registration succeded.\n");
	}
}

void RawInput::PostFrame()
{
	mouseDelta.first = 0;
	mouseDelta.second = 0;
	mouseButtonState[static_cast<size_t>(MouseButton::Wheel)] = false;
	wheelDelta = 0;
	
	if (clearNextFrame.first)
	{
		prevVirtualKeyState[clearNextFrame.second] = false;
		clearNextFrame = { false, 0 };
	}
}

long RawInput::GetMouseXDelta()
{
	return mouseDelta.first;
}

long RawInput::GetMouseYDelta()
{
	return mouseDelta.second;
}

bool RawInput::IsKeyDown(WPARAM virtualKeyCode)
{
	return virtualKeyState[virtualKeyCode];
}

bool RawInput::WasKeyDown(WPARAM virtualKeyCode)
{
	return (prevVirtualKeyState[virtualKeyCode] && !virtualKeyState[virtualKeyCode]);
}
