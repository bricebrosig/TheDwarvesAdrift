/// \file Component.h
/// \brief Interface for the component class CComponent.

#pragma once

//some forward declarations to make the compiler less stroppy

class CTimer;
class CRandom;
class CKeyboard;
class CXBoxController;
class CAudio;

/// \brief The component accessor.
///
/// This singleton class encapsulates the components commonly
/// needed to create a game: A timer, a PRNG, an audio player, 
/// a keyboard handler, and two particle engines. 
/// Classes that need these pointers simply need to
/// be derived from CComponent. 

class CComponent{
  protected:
    static CTimer* m_pTimer; ///< Pointer to a timer.
    static CRandom* m_pRandom; ///< Pointer to a PRNG.
    static CKeyboard* m_pKeyboard; ///< Pointer to a keyboard handler.
    static CXBoxController* m_pController; ///< Pointer to a XBox controller.
    static CAudio* m_pAudio; ///< Pointer to an audio player.
}; //CComponent
