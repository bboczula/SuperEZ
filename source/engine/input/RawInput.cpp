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
      mouseDelta.first += rawInput.data.mouse.lLastX;
      mouseDelta.second += rawInput.data.mouse.lLastY;
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
