/// \file Component.cpp
/// \brief Code for the component class CComponent.
///
/// This file contains declarations for a timer, a PRNG, 
/// an audio player, a keyboard handler, and
/// two particle engines, which are then used to initialize 
/// the corresponding static member variables of CComponent. 

#include "ComponentIncludes.h"

namespace{
  static CTimer cTimer;
  static CRandom cRandom;
  static CKeyboard cKeyboard;
  static CXBoxController cController;
  static CAudio cAudio;
} //namespace

CTimer* CComponent::m_pTimer = &cTimer;
CRandom* CComponent::m_pRandom = &cRandom;
CKeyboard* CComponent::m_pKeyboard = &cKeyboard;
CXBoxController* CComponent::m_pController = &cController; 
CAudio* CComponent::m_pAudio = &cAudio;
