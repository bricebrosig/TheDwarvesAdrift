/// \file Controller.h
/// \brief Interface for the XBox 360 controller class CXBoxController.

#pragma once

#include <windows.h>
#include <XInput.h>

#include "Defines.h"

/// \brief The XBox 360 controller class.
///
/// Polled controller class providing access to control states and
/// whether whether buttons have toggled (were up and went down)
/// since the last frame.

class CXBoxController{
  private:
    XINPUT_STATE m_strState; ///< Controller's current state (polled each frame).
    
    bool m_bButtonADown = false; ///< A button is down.
    bool m_bButtonBDown = false; ///< B button is down.
    bool m_bButtonXDown = false; ///< X button is down.
    bool m_bButtonYDown = false; ///< Y button is down.
    bool m_bButtonLSDown = false; ///< Left shoulder button is down.
    bool m_bButtonRSDown = false; ///< Right shoulder button is down.

    bool ButtonDown(DWORD button); ///< Check button state.

  public:
    void GetState(); ///< Get controller's current state.
    bool IsConnected(); ///< Query whether controller is connected.
    void Vibrate(WORD leftVal=0, WORD rightVal=0); ///< Set left and right rumble.
    
    Vector2 GetRThumb(); ///< Get position of right thumb stick.
    Vector2 GetLThumb(); ///< Get position of left thumb stick.
    
    float GetRTrigger(); ///< Get position of right trigger.
    float GetLTrigger(); ///< Get position of left trigger.

    bool GetButtonAToggle(); ///< Get button A toggle state.
    bool GetButtonBToggle(); ///< Get button B toggle state.
    bool GetButtonXToggle(); ///< Get button X toggle state.
    bool GetButtonYToggle(); ///< Get button Y toggle state.
    
    bool GetDPadRight(); ///< Get digital pad right button state.
    bool GetDPadLeft(); ///< Get digital pad left button state.
    bool GetDPadUp(); ///< Get digital pad up button state.
    bool GetDPadDown(); ///< Get digital pad down button state.
    
    bool GetButtonRSToggle(); ///< Get right shoulder button toggle.
    bool GetButtonLSToggle(); ///< Get left shoulder button toggle.
}; //CXBoxController
