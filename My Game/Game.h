/// \file Game.h
/// \brief Interface for the game class CGame.

#pragma once

#include "Component.h"
#include "Common.h"
#include "ObjectManager.h"
#include "Settings.h"

/// \brief The game class.

class CGame: 
  public CComponent, 
  public CSettings,
  public CCommon{ 

  private:
		
		int m_nCurrentLevel = 0; ///< what level
		int pauseMenuIndex = 0; ///< where we are in the pause menu
    int loseMenuIndex = 0; ///< where we are in the lsoe menu
		int mainMenuIndex = 0; ///< where we are in the main menu
		int winMenuIndex = 0; ///< where we are in the win menu
		float accumulator = 0.0f; ///< float for the accumulator in process frame
    vector<eSpriteType> menuOptions; ///< vector of the different menu options
    vector<eSpriteType> pauseOptions; ///< vector of the different pause options
    vector<eSpriteType> loseOptions; ///< vector of the different lose options
		vector<eSpriteType> winOptions; ///< Vector fo the different win screen options

    void BeginGame(); ///< Begin playing the game.
    void KeyboardHandler(); ///< The keyboard handler.
    void ControllerHandler(); ///< The controller handler.
    void RenderFrame(); ///< Render an animation frame.
    void CreateObjects(); ///< Create game objects.
    void FollowCamera(); ///< Make camera follow player character.

		void nextLevel(); ///< increments level counter and begins a game

  public:
    ~CGame(); ///< Destructor.

    void Initialize(); ///< Initialize the game.
    void ProcessFrame(); ///< Process an animation frame.
}; //CGame
