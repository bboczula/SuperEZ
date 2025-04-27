#pragma once

#include <Windows.h>
#include <windowsx.h>

#include "../Observer.h"

class WinMouse : public IObserver<WinMessageEvent>
{
public:
	WinMouse();
	~WinMouse();
	void OnNotify(WinMessageEvent& event) override;
	void OnMouseMove(const HWND& hwnd, const WPARAM& wParam, LPARAM& lParam);
	void OnSetCursor(LPARAM& lParam);
	void Initialize() override;
	void PostFrame() override;
	int getMousePositionX();
	int getMousePositionY();
private:
	short mousePosX, mousePosY;
	bool isTrackMouseEventRequired = true;
	POINTS lastPoint;
};