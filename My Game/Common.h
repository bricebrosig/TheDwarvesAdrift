/// \file Common.h
/// \brief Interface for the class CCommon.

#pragma once

#include "SndList.h"

enum gameState {
  TITLE_STATE, PAUSE_STATE, PLAY_STATE, WIN_STATE, LOSE_STATE,
  NUM_STATES
};

//forward declarations to make the compiler less stroppy

class CObjectManager; 
class CRenderer;
class CParticleEngine2D;
class CObject;

/// \brief The common variables class.
///
/// CCommon is a singleton class that encapsulates 
/// things that are common to different game components,
/// including game state variables.
/// Making it a singleton class means
/// that we can avoid passing its member variables
/// around as parameters, which makes the code
/// minisculely faster, and more importantly, reduces
/// function clutter.

class CCommon{
  protected:  
    static CRenderer* m_pRenderer; ///< Pointer to the renderer.
    static CObjectManager* m_pObjectManager; ///< Pointer to the object manager.
    static CParticleEngine2D* m_pParticleEngine; ///< Pointer to particle engine.    

    static bool m_bDrawAABBs; ///< Whether to draw AABBs.
    static Vector2 m_vWorldSize; ///< World height and width.
		static CObject* m_pPlayer; ///< Pointer to player character.
		static CObject* m_pShieldPlayer; ///< we wanna know where the shield player is so we can put his shield in the right place
		static vector<CObject*> characters; ///< vector of the three characters. m_pPlayer points at one of these
		static int whichPlayer; ///> index of the characters array for which player m_pPlayer is pointing at
		static float m_fGravity; ///< gravity effects the whole world, thus it goes in common
    static gameState state; ///< state of the game. That is: win, pause, title, and so on
    static float elevatorVel; ///< How fast the elevators should move
}; //CCommon
