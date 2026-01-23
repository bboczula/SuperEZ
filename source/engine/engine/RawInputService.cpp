#include "RawInputService.h"
#include "input/RawInput.h"

bool RawInputService::MouseClicked(InputMouseButton button)
{
	// Requires RawInput to expose:
	// - bool IsMouseButtonDown(MouseButton)
	// - bool WasMouseButtonDown(MouseButton)
	switch (button)
	{
	case InputMouseButton::Left:
		return raw_.IsLeftButtonDown();
	case InputMouseButton::Middle:
		return raw_.IsMiddleButtonDown();
	case InputMouseButton::Right:
		return raw_.IsRightButtonDown();
	}

	return false;
}

bool RawInputService::IsKeyDown(int keyCode)
{
	return raw_.IsKeyDown(static_cast<WPARAM>(keyCode));
}