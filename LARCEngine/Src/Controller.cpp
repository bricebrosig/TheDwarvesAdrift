/// \file Controller.cpp
/// \brief Code for the XBox 360 controller class CXBoxController.

#include "Controller.h"

/// This function should be called once per frame.

void CXBoxController::GetState(){
  ZeroMemory(&m_strState, sizeof(XINPUT_STATE));
  XInputGetState(0, &m_strState);
} //GetState

/// \return true if there is a controller connected.

bool CXBoxController::IsConnected(){
  ZeroMemory(&m_strState, sizeof(XINPUT_STATE));
  return XInputGetState(0, &m_strState) == ERROR_SUCCESS;
} //IsConnected

/// Note: calling this function with no parameters defaults to all stop.
/// \param leftVal Left vibrate speed from zero to max unsigned short.
/// \param rightVal Right vibrate speed from zero to max unsigned short.

void CXBoxController::Vibrate(WORD leftVal, WORD rightVal){
  XINPUT_VIBRATION Vibration;
  ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

  Vibration.wLeftMotorSpeed = leftVal;
  Vibration.wRightMotorSpeed = rightVal;

  XInputSetState(0, &Vibration);
} //Vibrate

/// Helper function that normalizes a vector representing a thumb
/// stick's position to [-1.0, 1.0]^2, clipping at the maximum value
///  and adjustingfor a dead zone at the middle position.
/// \param v Vector to be normalized.
/// \return Normalized vector.

Vector2 normalize(Vector2 v){
  if(v == Vector2(0.0f))
    return v;

  float len = v.Length();
  v /= len;
  
  if(len > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE){ //controller is outside a circular dead zone
    if(len > 32767) len = 32767; //clip magnitude at expected maximum value
    len -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE; //adjust relative to end of dead zone
    len /= (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);  //normalize to [0.0, 1.0]
  } //if

  else len = 0.0; //controller in deadzone 

  return len*v;
} //normalize

/// \return Normalized right thumb stick position.

Vector2 CXBoxController::GetRThumb(){
  const float x = m_strState.Gamepad.sThumbRX;
  const float y = m_strState.Gamepad.sThumbRY;

  return normalize(Vector2(x, y));
} //GetRThumb

/// \return Normalized left thumb stick position.
  
Vector2 CXBoxController::GetLThumb(){
  const float x = m_strState.Gamepad.sThumbLX;
  const float y = m_strState.Gamepad.sThumbLY;

  return normalize(Vector2(x, y));
} //GetLThumb

/// \return Right trigger position as a float between 0 and 1.

float CXBoxController::GetRTrigger(){
  return m_strState.Gamepad.bRightTrigger/255.0f;
} //GetRTrigger

/// \return Left trigger position as a float between 0 and 1.

float CXBoxController::GetLTrigger(){
  return m_strState.Gamepad.bLeftTrigger/255.0f;
} //GetRTrigger

/// true if button A was toggled.

bool CXBoxController::GetButtonAToggle(){
  const bool bWasDown = m_bButtonADown;
  m_bButtonADown = (m_strState.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
  return m_bButtonADown && !bWasDown;
} //GetButtonAToggle

/// true if button B was toggled.

bool CXBoxController::GetButtonBToggle(){
  const bool bWasDown = m_bButtonBDown;
  m_bButtonBDown = (m_strState.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
  return m_bButtonBDown && !bWasDown;
} //GetButtonBToggle

/// true if button X was toggled.

bool CXBoxController::GetButtonXToggle(){
  const bool bWasDown = m_bButtonXDown;
  m_bButtonXDown = (m_strState.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
  return m_bButtonXDown && !bWasDown;
} //GetButtonXToggle

/// true if button Y was toggled.

bool CXBoxController::GetButtonYToggle(){
  const bool bWasDown = m_bButtonYDown;
  m_bButtonYDown = (m_strState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
  return m_bButtonYDown && !bWasDown;
} //GetButtonYToggle

/// true if left shoulder button was toggled.

bool CXBoxController::GetButtonLSToggle(){
  const bool bWasDown = m_bButtonLSDown;
  m_bButtonLSDown = (m_strState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
  return m_bButtonLSDown && !bWasDown;
} //GetButtonLSToggle

/// true if right shoulder button was toggled.

bool CXBoxController::GetButtonRSToggle(){
  const bool bWasDown = m_bButtonRSDown;
  m_bButtonRSDown = (m_strState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
  return m_bButtonRSDown && !bWasDown;
} //GetButtonRSToggle

/// \param button Index of a button.
/// \return true if that button is down.

bool CXBoxController::ButtonDown(DWORD button){
  return (m_strState.Gamepad.wButtons & button) != 0;
} //ButtonDown

/// \return true if digital pad left button is down.

bool CXBoxController::GetDPadLeft(){
  return ButtonDown(XINPUT_GAMEPAD_DPAD_LEFT);
} //GetDPadLeft

/// \return true if digital pad right button is down.

bool CXBoxController::GetDPadRight(){
  return ButtonDown(XINPUT_GAMEPAD_DPAD_RIGHT);
} //GetDPadRight

/// \return true if digital pad up button is down.

bool CXBoxController::GetDPadUp(){
  return ButtonDown(XINPUT_GAMEPAD_DPAD_UP);
} //GetDPadUp

/// \return true if digital pad down button is down.

bool CXBoxController::GetDPadDown(){
  return ButtonDown(XINPUT_GAMEPAD_DPAD_DOWN);
} //GetDPadDown
