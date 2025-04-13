#include "WinMouse.h"

WinMouse::WinMouse()
{
}

WinMouse::~WinMouse()
{
}

void WinMouse::OnNotify(WinMessageEvent& event)
{
      switch (event.msg)
      {
      // Sent to a window if the mouse causes the cursor to move within a window
      // and mouse input is not captured.
      case WM_SETCURSOR:
      {
            OnSetCursor(event.lParam);
            break;
      }
      // Sent to a window after it has gained the keyboard focus
      case WM_SETFOCUS:
            break;
      // Sent to a window immediately before it loses the keyboard focus.
      case WM_KILLFOCUS:
            break;
      case WM_MOUSEMOVE:
      {
            OnMouseMove(event.hwnd, event.wParam, event.lParam);
            break;
      }
      case WM_LBUTTONDOWN:
      {
            int xPos = GET_X_LPARAM(event.lParam);
            int yPos = GET_Y_LPARAM(event.lParam);
            break;
      }
      case WM_MOUSELEAVE:
      {
            isTrackMouseEventRequired = true;
            break;
      }
      case WM_MOUSEHOVER:
      {
            isTrackMouseEventRequired = true;
            break;
      }
      case WM_NCMOUSELEAVE:
      {
            isTrackMouseEventRequired = true;
            break;
      }
      case WM_NCMOUSEHOVER:
      {
            isTrackMouseEventRequired = true;
            break;
      }
      default:
      	break;
      }
}

void WinMouse::OnMouseMove(const HWND& hwnd, const WPARAM& wParam, LPARAM& lParam)
{
      // Track mouse event
      if (isTrackMouseEventRequired)
      {
            TRACKMOUSEEVENT e = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, HOVER_DEFAULT };
            TrackMouseEvent(&e);
            isTrackMouseEventRequired = false;
      }

      // Capture the mouse in case the user wants to drag it outside
      if ((wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) == 0)
      {
            // Only release the capture if we really have it
            if (GetCapture() == hwnd)
                  ReleaseCapture();
      }
      else if (GetCapture() != hwnd)
      {
            // Set the capture to continue receiving mouse events
            SetCapture(hwnd);
      }

      // Get the client area of the window
      RECT area;
      GetClientRect(hwnd, &area);

      POINTS point = MAKEPOINTS(lParam);
      POINT p = { point.x, point.y };
      ClientToScreen(hwnd, &p);
      lastPoint = point;

      // If the cursor is outside the client area...
      if ((point.x < area.left) || (point.x > area.right) || (point.y < area.top) || (point.y > area.bottom))
      {
            // Sapphire::Logger::GetInstance().Log("Outside client area\n");
      }
      else
      {
            // Sapphire::Logger::GetInstance().Log("Inside client area\n");
      }
}

void WinMouse::OnSetCursor(LPARAM& lParam)
{
      // The mouse has moved, if the cursor is in our window we must refresh the cursor
      if (LOWORD(lParam) == HTCLIENT)
      {
            // Sets the cursor shape
            HCURSOR cursor = LoadCursor(nullptr, IDC_CROSS);
            SetCursor(cursor);
      }
}

void WinMouse::Initialize()
{
}

void WinMouse::PostFrame()
{
}

int WinMouse::getMousePositionX()
{
      return mousePosX;
}

int WinMouse::getMousePositionY()
{
      return mousePosY;
}
