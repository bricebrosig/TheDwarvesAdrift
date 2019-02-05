/// \file Window.h
/// \brief Interface for the window descriptor CWindowDesc.

#pragma once

#include <Windows.h>

#include "Defines.h"

/// \brief The window descriptor.
///
/// CWindowDesc is a singleton class containing
/// the game's instance handle and the window handle to
/// the primary (in fact the only) window created by this
/// application. Making it a singleton class means
/// that we can avoid passing its member variables
/// around as parameters, which makes the code
/// minisculely faster, and more importantly, reduces
/// function clutter.

class CWindowDesc{
  protected: 
    static HWND m_Hwnd; ///< Window handle.
    static HINSTANCE m_hInst; ///< Instance handle.
}; //CWindowDesc
