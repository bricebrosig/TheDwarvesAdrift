/// \file Window.h
/// \brief Interface for the window class CWindow.

#pragma once

#include <Windows.h>

#include "Defines.h"
#include "tinyxml2.h"
#include "Settings.h"
#include "WindowDesc.h"

/// \brief The window class. 
///
/// CWindow handles Windows specific things. This will help
/// you to keep the rest of your code as OS-independent as
/// possible. Gnarly things that it handles includes making
/// a window whose client area is a specific width and height,
/// and making sure that the window maintains its aspect ration
/// if the user is brave enough to resize it by dragging on
/// one of the window's edges or corners.

class CWindow: 
  public CWindowDesc,
  public CSettingsManager{

  private:
    static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM wp, LPARAM lp); ///< The Window Procedure.

  private:
    HWND CreateGameWindow(HINSTANCE h); ///< Create default window.
    static void GetBorderSizes(int& w, int& h); ///< Get window border width and height.

    static void EnforceAspectRatio(WPARAM wParam, RECT* pRect); ///< Enforce aspect ratio.
    static void EnforceAspectRatio(MINMAXINFO* pmmi); ///< Enforce aspect ratio.

  public:
    BOOL WinMain(_In_ HINSTANCE hInstance);  ///< Default WinMain.
}; //CWindow
