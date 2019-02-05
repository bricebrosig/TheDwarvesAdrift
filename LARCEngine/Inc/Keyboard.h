/// \file Keyboard.h
/// \brief Interface for the keyboard handler class CKeyboard.

#pragma once

#include "WindowDesc.h"

/// \brief The keyboard handler.
///
/// This is a polled keyboard handler. Its GetState() function
/// must be called once per frame. The keyboard state from
/// the previous frame is retained so that queries can determine
/// whether a key changed state.

class CKeyboard: public CWindowDesc{
  static const int NUMKEYS = 256; ///< Number of keys on the keyboard.

  private:
    unsigned char m_pState[NUMKEYS]; ///< Space for current state of keys.
    unsigned char m_pOldState[NUMKEYS]; ///< Space for current state of keys.

    bool Trigger(const WPARAM k, bool bDown); ///< Check for key changing state.
    bool Trigger(bool bDown); ///< Check for any key changing state.

  public:
    CKeyboard(); ///< Constructor.

    void GetState(); ///< Poll the keyboard state.

    bool Down(const WPARAM k); ///< Check for key down.

    bool TriggerDown(const WPARAM k); ///< Check for key changing from up to down.
    bool TriggerUp(const WPARAM k); ///< Check for key changing from down to up.
    
    bool TriggerDown(); ///< Check for any key changing from up to down.
    bool TriggerUp(); ///< Check for any key changing from down to up.
}; //CKeyboard
