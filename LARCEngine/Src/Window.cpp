/// \file Window.cpp
/// \brief Code for the window class CWindow.

#include "Window.h"

/// Register and create a window. Care is taken to ensure that the
/// client area of the window is the right size, because the default
/// way of creating a window has you specify the width and height
/// including the frame.
/// \param h handle to the current instance of this application.
/// \return Handle to the application window.

HWND CWindow::CreateGameWindow(HINSTANCE h){
  m_hInst = h; //save the instance handle because we might need it

  WNDCLASS wc; //window registration info

  //fill in registration information wc
  wc.style = CS_HREDRAW | CS_VREDRAW; //style
  wc.lpfnWndProc = WndProc; //window message handler
  wc.cbClsExtra = wc.cbWndExtra = 0;
  wc.hInstance = h; //instance handle
  wc.hIcon = nullptr; //default icon
  wc.hCursor = LoadCursor(NULL, IDC_ARROW); //arrow cursor
  wc.hbrBackground = nullptr; //we will draw background
  wc.lpszMenuName = nullptr; //no menu
  wc.lpszClassName = m_szName; //game name
            
  RegisterClass(&wc); //register window

  //create and set up window 
  RECT r;  
  r.left = 0; r.right = m_nWinWidth; 
  r.top = 0; r.bottom = m_nWinHeight;

  DWORD dwStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME | WS_SYSMENU; 
  DWORD dwStyleEx = WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME;

  AdjustWindowRectEx(&r, dwStyle, FALSE, dwStyleEx); 

  ////make sure that window is not bigger than screen

  //window width and height after adjustment
  float winw = (float)r.right - r.left;
  float winh = (float)r.bottom - r.top;

  //screen width and height
  const float sw = (float)GetSystemMetrics(SM_CXSCREEN);
  const float sh = (float)GetSystemMetrics(SM_CYSCREEN);

  //ready to create window

  m_Hwnd = CreateWindowEx(dwStyleEx, m_szName, m_szName, dwStyle, 0, 0,
    (int)winw, (int)winh, nullptr, nullptr, h, nullptr);

  //center window on screen
  float x = (sw - winw)/2.0f;
  float y = (sh - winh)/2.0f;
  SetWindowPos(m_Hwnd, nullptr, (int)x, (int)y, 
    (int)winw, (int)winh, SWP_NOZORDER | SWP_SHOWWINDOW);

  if(m_Hwnd){ //if successfully created window
    ShowWindow(m_Hwnd, true); 
    UpdateWindow(m_Hwnd); 
    SetFocus(m_Hwnd); //bind keyboard
  } //if

  return m_Hwnd; //return window handle
} //CreateGameWindow

/// Get the combined width and height of the borders on the window.
/// Combined width is the sum of the widths of the left and
/// right borders. Combined height is the sum of the heights
/// of the top and bottom borders.
/// \param w [OUT] Combined border width.
/// \param h [OUT] Combined border height.

void CWindow::GetBorderSizes(int& w, int& h){
  RECT cr; //client rectangle
  RECT wr; //window rectangle, includes client rectangle and borders

  GetClientRect(m_Hwnd, &cr);
  GetWindowRect(m_Hwnd, &wr);

  //combined border width and height
  w = (wr.right - wr.left) - (cr.right - cr.left); //border width
  h = (wr.bottom - wr.top) - (cr.bottom - cr.top); //border height
} //GetBorderSizes

/// Force the aspect ratio of the client area of the window
/// in response to a WM_SIZING message. This function
/// resizes the drag rectangle provided by the WM_SIZING message.
/// \param wParam WMSZ message telling us which edge is being dragged on.
/// \param pRect [IN, OUT] Pointer to drag rectangle.

void CWindow::EnforceAspectRatio(WPARAM wParam, RECT* pRect){ 
  RECT& dr = *pRect; //drag rectangle

  int bw, bh; //border width and height
  GetBorderSizes(bw, bh);

  //new drag window width and height
  int dw = bw + int((dr.bottom - dr.top - bh)*m_fAspectRatio); //new drag rectangle width
  int dh = bh + int((dr.right - dr.left - bw)/m_fAspectRatio); //new drag rectangle ht

  //fix aspect ratio of drag rectangle
  switch(wParam){ //what edge are we dragging on?
    case WMSZ_LEFT:
    case WMSZ_RIGHT:
		//dr.top = (dr.top + dr.bottom - dh)/2;
		dr.bottom = dr.top + dh;
    break;
      
    case WMSZ_TOP:
    case WMSZ_BOTTOM:
      //dr.left = (dr.left + dr.right - dw)/2;
      dr.right = dr.left + dw;
    break;
      
    case WMSZ_TOPRIGHT:
    case WMSZ_TOPLEFT:
      if(dh > bh)
        dr.top = dr.bottom - dh;
      else
        dr.right = dr.left - dw;
     break;

    case WMSZ_BOTTOMRIGHT:
    case WMSZ_BOTTOMLEFT:
      if(dh > bh)
        dr.bottom = dr.top + dh;
      else 
        dr.right = dr.left + dw;
     break;
  } //switch
} //EnforceAspectRatio

/// Enforce the aspect ratio in response to a WM_GETMINMAXINFO
/// message. This function ensures that when the window is
/// bigger than the screen, then it has the correct aspect
/// ratio when Windows automatically resizes it.
/// \param pmmi [OUT] Pointer to a MINMAXINFO structure.

void CWindow::EnforceAspectRatio(MINMAXINFO* pmmi) {
  int bw, bh; //border width and height
  GetBorderSizes(bw, bh);

  //Enforce maximum size with correct aspect ratio

	POINT p;
	p.x = GetSystemMetrics(SM_CXSCREEN);
	p.y = GetSystemMetrics(SM_CYSCREEN);

	const int dx = int((p.y - bh)*m_fAspectRatio) + bw;
	const int dy = int((p.x - bw)/m_fAspectRatio) + bh;

  pmmi->ptMaxTrackSize.x = min(dx, p.x);
  pmmi->ptMaxTrackSize.y = min(p.y, dy);

  pmmi->ptMinTrackSize.x = m_nWinWidth/2 + bw;
  pmmi->ptMinTrackSize.y = m_nWinHeight/2 + bh;
} //EnforceAspectRatio

/// \brief Default window procedure.
/// This is the handler for messages from the operating system. 
/// \param h Window handle.
/// \param m Message code.
/// \param wp Parameter for message.
/// \param lp Second parameter for message.
/// \return 0 If message is handled.

LRESULT CALLBACK CWindow::WndProc(HWND h, UINT m, WPARAM wp, LPARAM lp){
  switch(m){ //handle message
    case WM_DESTROY: //on exit
      PostQuitMessage(0); //this is the last thing to do on exit
    break;

	case WM_SIZING: //user is resizing the window
	  EnforceAspectRatio(wp, (RECT*)lp);
		return TRUE;

	case WM_GETMINMAXINFO:
		EnforceAspectRatio((MINMAXINFO*)lp);
		return TRUE;

    default:
      return DefWindowProc(h, m, wp, lp); //default window procedure
  } //switch

  return FALSE;
} //WndProc

/// The main entry point for this application should call this function first. 
/// \param hInstance Handle to the current instance of this application.
/// \return TRUE if application terminates correctly.

BOOL CWindow::WinMain(_In_ HINSTANCE hInstance){
  Load(); //load game settings from xml file

  HWND hwnd = CreateGameWindow(hInstance); //create window
  if(!hwnd)return FALSE; //bail if problem creating window

  return TRUE;
} //wWinMain