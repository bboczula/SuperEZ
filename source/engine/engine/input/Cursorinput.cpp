#include "CursorInput.h"
#include <windowsx.h> // This gives you GET_X_LPARAM, GET_Y_LPARAM


void CursorInput::OnNotify(WinMessageEvent& event)
{
      if (event.msg == WM_LBUTTONUP)
      {
		leftButtonClicked = true;
      }

      if (event.msg == WM_MOUSEMOVE)
      {
            mouseX = GET_X_LPARAM(event.lParam);
            mouseY = GET_Y_LPARAM(event.lParam);
      }
}
