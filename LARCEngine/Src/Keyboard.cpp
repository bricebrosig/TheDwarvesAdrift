/// \file Keyboard.cpp
/// \brief Code for the keyboard handler class CKeyboard.

#include "Keyboard.h"

/// The constructor initializes all key states to zero.

CKeyboard::CKeyboard(){   
  for(int i=0; i<NUMKEYS; i++)
    m_pState[i] = m_pOldState[i] = 0x00;
} //constructor

/// Use the Windows API GetKeyboardState function to get the
/// state of all 256 keys on the keyboard. The results are
/// stored as an array of bytes in m_pState, while the
/// previous state is also stored in m_pOldState. If the
/// ESC key has been pressed, then the program is shut down
/// by calling the Windows API function DestroyWindow.

void CKeyboard::GetState(){
  CopyMemory(m_pOldState, m_pState, sizeof(m_pState)); //copy to old state
  GetKeyboardState(m_pState); //get current state
    
  if(TriggerDown(VK_ESCAPE)) //ESC key was pressed
    DestroyWindow(m_Hwnd); //shut down this whole kit & kaboodle
} //GetState

/// Check whether a key is currently down.
/// This function assumes that GetState() has been called recently
/// to get the current keyboard state. 
/// \param k Virtual key code of key to check.
/// \return True if key k is down.

bool CKeyboard::Down(const WPARAM k){  
  return (m_pState[k] & 0x80) == 0x80;
} //Down

/// Check whether a key has just changed state, ie. whether it
/// just went up or just went down. This function assumes that 
/// GetState() has been called recently to get the current 
/// keyboard state. 
/// \param k Virtual key code of key to check.
/// \param bDown True if we are checking for key down, false if we are checking for key up.
/// \return True if key k was changed to the desired state.

bool CKeyboard::Trigger(const WPARAM k, bool bDown){
  const bool bIsDown = (m_pState[k] & 0x80) == 0x80; //is it up now?
  const bool bStateChanged = m_pState[k] != m_pOldState[k]; //did it change state?
  return bStateChanged && (bIsDown == bDown);
} //Trigger

/// Check whether a key went up. This function assumes that 
/// GetState() has been called recently to get the current 
/// keyboard state. 
/// \param k Virtual key code of key to check.
/// \return True if key k went up.

bool CKeyboard::TriggerUp(const WPARAM k){
  return Trigger(k, false);
} //TriggerUp

/// Check whether a key went down. This function assumes that 
/// GetState() has been called recently to get the current 
/// keyboard state. 
/// \param k Virtual key code of key to check.
/// \return True if key k went down.

bool CKeyboard::TriggerDown(const WPARAM k){
  return Trigger(k, true);
} //TriggerDown

/// Check whether any key changed state in a particular 
/// direction, not including the ESC key or the function keys
/// F1 through F12.
/// This function assumes that GetState() has been called 
/// recently to get the current keyboard state. 
/// \param bDOwn True if we're checking for key down, false if up.
/// \return True if a key changed state.

bool CKeyboard::Trigger(bool bDown){
  for(int i=VK_BACK; i<VK_F1; i++)
    if(Trigger(i, bDown)) 
      return true;
  
  for(int i=VK_F12+1; i<NUMKEYS; i++)
    if(Trigger(i, bDown)) 
      return true;

  return false;
} //Trigger

/// Check whether any key went up, not including the ESC
/// key or the function keys F1 through F12. This includes
/// the mouse buttons.
/// This function assumes that GetState() has been called 
/// recently to get the current keyboard state. 
/// \return True if a key went up.

bool CKeyboard::TriggerUp(){
  return Trigger(true);
} //TriggerUp

/// Check whether any key went down, not including the ESC
/// key or the function keys F1 through F12. This includes
/// the mouse buttons.
/// This function assumes that GetState() has been called 
/// recently to get the current keyboard state. 
/// \return True if a key went down.

bool CKeyboard::TriggerDown(){
  return Trigger(true);
} //TriggerDown