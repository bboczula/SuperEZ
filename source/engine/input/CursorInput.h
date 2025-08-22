#pragma once

#include "../Observer.h"
#include <Windows.h>

// Simple class for absolute mouse position (client coordinates)
class CursorInput : public IObserver<WinMessageEvent>
{
public:
	void OnNotify(WinMessageEvent& event) override;
	void Initialize() override {}
	void PostFrame() override { leftButtonClicked = false; }

	LONG GetMouseX() const { return mouseX; }
	LONG GetMouseY() const { return mouseY; }
	bool WasLeftButtonClicked() const { return leftButtonClicked; }

private:
	LONG mouseX = 0;
	LONG mouseY = 0;
	bool leftButtonClicked = false;
};