/// \file Game.cpp
/// \brief Code for the game class CGame.

#include "Game.h"

#include "GameDefines.h"
#include "Renderer.h"
#include "ComponentIncludes.h"
#include "ParticleEngine.h"

#include "DebugPrintf.h"

#define NUM_LEVELS 5

/// Delete the renderer and the object manager.

CGame::~CGame(){
  delete m_pParticleEngine;
  delete m_pRenderer;
  delete m_pObjectManager;
} //destructor

/// Initialize the renderer and the object manager, load 
/// images and sounds, and begin the game.
/// sets the state and initializes all the 
/// vectors for the different options in the different menus

void CGame::Initialize(){
  m_pRenderer = new CRenderer; 
  m_pRenderer->Initialize(NUM_SPRITES); 
  m_pRenderer->LoadImages(); //load images from xml file list

	accumulator = 0.0f;

  state = TITLE_STATE;
  m_nCurrentLevel = 0;
	mainMenuIndex = 0;
  pauseMenuIndex = 0;
	winMenuIndex = 0;

	//push back the different menu options onto the vector
	//push more as we add more buttons
  menuOptions = vector<eSpriteType>();
  menuOptions.push_back(START_BUTTON);
  menuOptions.push_back(EXIT_BUTTON);

	//same for pause menu
  pauseOptions = vector<eSpriteType>();
	pauseOptions.push_back(CONTINUE_BUTTON);
	pauseOptions.push_back(MENU_BUTTON);
	pauseOptions.push_back(EXIT_BUTTON);
	pauseOptions.push_back(RESTART_SPRITE);

  //same for lose menu
  loseOptions = vector<eSpriteType>();
  loseOptions.push_back(MENU_BUTTON);
  loseOptions.push_back(EXIT_BUTTON);
  loseOptions.push_back(RESTART_SPRITE);

	winOptions = vector<eSpriteType>();
	winOptions.push_back(MENU_BUTTON);
	winOptions.push_back(EXIT_BUTTON);

  m_pObjectManager = new CObjectManager; //set up the object manager 
  m_pAudio->Load(); //load the sounds for this game

  m_pParticleEngine = new CParticleEngine2D((CSpriteRenderer*)m_pRenderer);
	BeginGame();
} //Initialize

/// Ask the object manager to create a player object
/// and a turret.

void CGame::CreateObjects(){
} //CreateObjects

/// Call this function to start a new game. This
/// should be re-entrant so that you can start a
/// new game without having to shut down and restart the
/// program

void CGame::BeginGame(){
  m_pParticleEngine->clear(); //clear old particles
  m_pObjectManager->clear(); //clear old objects
  m_pObjectManager->LoadMap(m_nCurrentLevel); //load map
  CreateObjects(); //create new objects
} //BeginGame

/// Poll the keyboard state and respond to the
/// key presses that happened since the last frame.

void CGame::KeyboardHandler(){
  m_pKeyboard->GetState(); //get current keyboard state 

	//side to side movement
	if (m_pKeyboard->Down('D') && m_pKeyboard->Down('A')){
		/*dont fucking move*/
	}
	else {
		if (m_pKeyboard->Down('D')) {
			m_pPlayer->StrafeRight();
			m_pPlayer->turn(RIGHT);
		}
		if (m_pKeyboard->Down('A')) {
			m_pPlayer->StrafeLeft();
			m_pPlayer->turn(LEFT);
		}
	}

	//reset back to stationary
	if (m_pKeyboard->TriggerUp('D')){
		m_pPlayer->m_nCurrentFrame = 0;
	}
	if (m_pKeyboard->TriggerUp('A')){
		m_pPlayer->m_nCurrentFrame = 4;
	}

	//jump/climb up and down
	CObject* elev = m_pObjectManager->isTouchingElevator();
	if (m_pKeyboard->TriggerDown('W'))
	{
		if (m_pPlayer->m_nSpriteIndex == FAST_SPRITE && elev == nullptr) {
			m_pPlayer->Jump();
		}
		else if (elev != nullptr){
			((Elevator*)elev)->goUp();
		}
	}

	if (m_pKeyboard->Down('W'))
	{
		if (m_pPlayer->isOnLadder()) {
			m_pPlayer->climbUp();
		}
    else if (elev != nullptr) {
      ((Elevator*)elev)->goUp();
    }
	}

	if (m_pKeyboard->Down('S'))
	{
		if (m_pPlayer->isOnLadder())
		{
			m_pPlayer->climbDown();
		}
		else if (elev != nullptr) {
			((Elevator*)elev)->goDown();
		}
	}

	if (m_pPlayer->m_nSpriteIndex == FIGHTER_SPRITE && state == PLAY_STATE)
	{
		if (m_pKeyboard->TriggerDown(VK_SPACE))
			m_pObjectManager->swingSword();
		if(m_pKeyboard->TriggerDown('F'))
			if (m_pTimer->elapsed(m_pPlayer->m_fGunTimer, 1.5f))
				m_pObjectManager->FireGun(m_pPlayer, BULLET_SPRITE);
	}

	if (m_pPlayer->m_nSpriteIndex == SHIELD_SPRITE && state == PLAY_STATE)
	{
		if (m_pKeyboard->TriggerDown(VK_SPACE))
			m_pObjectManager->flipShield();
	}

  if(m_pKeyboard->TriggerDown(VK_BACK))
    BeginGame();

  if(m_pKeyboard->TriggerDown(VK_F1))
    m_bDrawAABBs = !m_bDrawAABBs;

	//switch active player
	if (m_pKeyboard->TriggerDown(VK_TAB)){
		whichPlayer = (whichPlayer + 1) % characters.size();
		m_pPlayer = characters.at(whichPlayer);
	}

	if (m_pKeyboard->TriggerDown(VK_F3)) {
		nextLevel();
	}

	//an efficient way to handle keyboard input in different gamestates
	switch (state)
	{
		case TITLE_STATE://controls for the menu navigation
			if (m_pKeyboard->TriggerDown(VK_UP)) {
				//decrement position in the options vector and mod by the size
				mainMenuIndex = (mainMenuIndex + 1) % menuOptions.size();
			}
			if(m_pKeyboard->TriggerDown(VK_DOWN)){
				//increment position in the options vector and mod by the size
				mainMenuIndex = (mainMenuIndex + 1) % menuOptions.size();
			}
			if (m_pKeyboard->TriggerDown(VK_RETURN)) { //enter which ever option is selected
				if (menuOptions.at(mainMenuIndex) == START_BUTTON)//begin game
				{
					state = PLAY_STATE;
					m_nCurrentLevel = 0;
					BeginGame();
				}
				if (menuOptions.at(mainMenuIndex) == EXIT_BUTTON)//exit the game
				{
					PostQuitMessage(0); //leave game with that exit code
				}
			}
			break;
    case LOSE_STATE:
      if (m_pKeyboard->TriggerDown(VK_UP)) {
        //decrement position in the options vector and mod by the size
        loseMenuIndex = (loseMenuIndex - 1) % loseOptions.size();
      }
      if (m_pKeyboard->TriggerDown(VK_DOWN)) {
        //increment position in the options vector and mod by the size
        loseMenuIndex = (loseMenuIndex + 1) % loseOptions.size();
      }
      if (m_pKeyboard->TriggerDown(VK_RETURN)) { //enter which ever option is selected
        if (loseOptions.at(loseMenuIndex) == MENU_BUTTON)//begin game
        {
          state = TITLE_STATE;
        }
        if (loseOptions.at(loseMenuIndex) == EXIT_BUTTON)//exit the game
        {
          PostQuitMessage(0); //leave game with that exit code
        }
        if (loseOptions.at(loseMenuIndex) == RESTART_SPRITE)
        {
          BeginGame();
          state = PLAY_STATE;
        }
      }break;
		case PAUSE_STATE:
			if (m_pKeyboard->TriggerDown(VK_UP)) {
				//decrement position in the options vector and mod by the size
				pauseMenuIndex = (pauseMenuIndex - 1) % pauseOptions.size();
			}
			if (m_pKeyboard->TriggerDown(VK_DOWN)) {
				//increment position in the options vector and mod by the size
				pauseMenuIndex = (pauseMenuIndex + 1) % pauseOptions.size();
			}
			if (m_pKeyboard->TriggerDown(VK_RETURN)) { //enter which ever option is selected
				if (pauseOptions.at(pauseMenuIndex) == CONTINUE_BUTTON)//begin game
				{
					state = PLAY_STATE;
				}
				if (pauseOptions.at(pauseMenuIndex) == MENU_BUTTON)//begin game
				{
					state = TITLE_STATE;
				}
				if (pauseOptions.at(pauseMenuIndex) == EXIT_BUTTON)//exit the game
				{
					PostQuitMessage(0); //leave game with that exit code
				}
				if (pauseOptions.at(pauseMenuIndex) == RESTART_SPRITE)
				{
					BeginGame();
					state = PLAY_STATE;
				}
			}
			break;
		case WIN_STATE:
			if (m_pKeyboard->TriggerDown(VK_UP)) {
				//decrement position in the options vector and mod by the size
				winMenuIndex = (winMenuIndex + 1) % winOptions.size();
			}
			if (m_pKeyboard->TriggerDown(VK_DOWN)) {
				//increment position in the options vector and mod by the size
				winMenuIndex = (winMenuIndex + 1) % winOptions.size();
			}
			if (m_pKeyboard->TriggerDown(VK_RETURN)) { //enter which ever option is selected
				if (winOptions.at(winMenuIndex) == MENU_BUTTON)//begin game
				{
					state = TITLE_STATE;
				}
				if (winOptions.at(winMenuIndex) == EXIT_BUTTON)//exit the game
				{
					PostQuitMessage(0); //leave game with that exit code
				}
			}
			break;
		default:
			break;
	}//switch

  if (m_pKeyboard->TriggerDown('P'))
  {
    switch (state)
    {
      case PAUSE_STATE:
      {
        state = PLAY_STATE;
        break;
      }
      case WIN_STATE:
      {
        state = TITLE_STATE;
        break;
      }
      case PLAY_STATE:
      {
        state = PAUSE_STATE;
        break;
      }
			case LOSE_STATE:
			{
				state = PLAY_STATE;
				m_pAudio->stop();
				BeginGame();
				break;
			}
    }
  }

	m_pPlayer->m_bInteract = false;
	if (m_pKeyboard->TriggerDown('E')) {
		m_pPlayer->m_bInteract = true;
	}
} //KeyboardHandler

/// Poll the XBox controller state and respond to the
/// controls there.

void CGame::ControllerHandler(){
	CObject* elev = m_pObjectManager->isTouchingElevator();
	if(!m_pController->IsConnected())return;

	m_pController->GetState(); //get state of controller's controls 

	//side to side movement
	if (state == PLAY_STATE)
	{
		if (m_pController->GetLThumb().x > .5f)
		{
			m_pPlayer->StrafeRight();
			m_pPlayer->turn(RIGHT);
		}
		else if (m_pController->GetLThumb().x < -.5f)
		{
			m_pPlayer->StrafeLeft();
			m_pPlayer->turn(LEFT);
		}

		//up and down
		if (m_pController->GetLThumb().y > .5f)
		{
			if (m_pPlayer->isOnLadder()) {
				m_pPlayer->climbUp();
			}
			else if (elev != nullptr) {
				((Elevator*)elev)->goUp();
			}
		}
		else if (m_pController->GetLThumb().y < -.5f)
		{
			if (m_pPlayer->isOnLadder())
			{
				m_pPlayer->climbDown();
			}
			else if (elev != nullptr) {
				((Elevator*)elev)->goDown();
			}
		}
	}

	//jump/ abilities
	if (m_pController->GetButtonAToggle()) //everything for the a button...
	{
		if (state == PLAY_STATE)
		{
			if (m_pPlayer->m_nSpriteIndex == FAST_SPRITE && elev == nullptr) {
				m_pPlayer->Jump();
			}
			else if (m_pPlayer->m_nSpriteIndex == FIGHTER_SPRITE && state == PLAY_STATE)
			{
				if (m_pTimer->elapsed(m_pPlayer->m_fGunTimer, 1.5f))
					m_pObjectManager->FireGun(m_pPlayer, BULLET_SPRITE);
			}
			else if (m_pPlayer->m_nSpriteIndex == SHIELD_SPRITE && state == PLAY_STATE)
			{
				m_pObjectManager->flipShield();
			}
			else if (elev != nullptr) {
				((Elevator*)elev)->goUp();
			}
		}

		switch (state)
		{
			case TITLE_STATE:
			{
				if (menuOptions.at(mainMenuIndex) == START_BUTTON)//begin game
				{
					state = PLAY_STATE;
					m_nCurrentLevel = 0;
					BeginGame();
				}
				if (menuOptions.at(mainMenuIndex) == EXIT_BUTTON)//exit the game
				{
					PostQuitMessage(0); //leave game with that exit code
				}
			}break;
			case PAUSE_STATE:
			{
				if (pauseOptions.at(pauseMenuIndex) == CONTINUE_BUTTON)//begin game
				{
					state = PLAY_STATE;
				}
				if (pauseOptions.at(pauseMenuIndex) == MENU_BUTTON)//begin game
				{
					state = TITLE_STATE;
				}
				if (pauseOptions.at(pauseMenuIndex) == EXIT_BUTTON)//exit the game
				{
					PostQuitMessage(0); //leave game with that exit code
				}
				if (pauseOptions.at(pauseMenuIndex) == RESTART_SPRITE)
				{
					BeginGame();
					state = PLAY_STATE;
				}
			}break;
			case LOSE_STATE:
			{
				if (loseOptions.at(loseMenuIndex) == MENU_BUTTON)//begin game
				{
					state = TITLE_STATE;
				}
				if (loseOptions.at(loseMenuIndex) == EXIT_BUTTON)//exit the game
				{
					PostQuitMessage(0); //leave game with that exit code
				}
				if (pauseOptions.at(pauseMenuIndex) == RESTART_SPRITE)
				{
					BeginGame();
					state = PLAY_STATE;
				}
			}break;
			case WIN_STATE:
			{
				if (winOptions.at(winMenuIndex) == MENU_BUTTON)//begin game
				{
					state = TITLE_STATE;
				}
				if (winOptions.at(winMenuIndex) == EXIT_BUTTON)//exit the game
				{
					PostQuitMessage(0); //leave game with that exit code
				}
			}break;
			default:
				break;
		}//switch
	}//if toggle A

	//b button
	if (m_pController->GetButtonBToggle() && state == PLAY_STATE) // everything for the b button
	{
		if (m_pPlayer->m_nSpriteIndex == FIGHTER_SPRITE)
		{
			m_pObjectManager->swingSword();
		}
	}//if b button

	//switch active player and change menu options
	if (m_pController->GetButtonRSToggle())
	{
		if (state == PLAY_STATE)
		{
			whichPlayer = (whichPlayer + 1) % characters.size();
			m_pPlayer = characters.at(whichPlayer);
		}

		switch (state)
		{
			case TITLE_STATE:
			{
				mainMenuIndex = (mainMenuIndex + 1) % menuOptions.size();
			}break;
			case PAUSE_STATE:
			{
				pauseMenuIndex = (pauseMenuIndex + 1) % pauseOptions.size();
			}break;
			case LOSE_STATE:
			{
				loseMenuIndex = (loseMenuIndex + 1) % loseOptions.size();
			}break;
			case WIN_STATE:
			{
				winMenuIndex = (winMenuIndex + 1) % winOptions.size();
			}break;
			default:
				break;
		}//switch
	}// y button

	if (m_pController->GetButtonLSToggle())
	{
		if (state == PLAY_STATE)
		{
			whichPlayer = (whichPlayer - 1) < 0 ? (characters.size() - 1) : (whichPlayer - 1);
			//whichPlayer = (whichPlayer - 1) % characters.size();
			m_pPlayer = characters.at(whichPlayer);
		}

		switch (state)
		{
		case TITLE_STATE:
		{
			mainMenuIndex = (mainMenuIndex - 1) % menuOptions.size();
		}break;
		case PAUSE_STATE:
		{
			pauseMenuIndex = (pauseMenuIndex - 1) % pauseOptions.size();
		}break;
		case LOSE_STATE:
		{
			loseMenuIndex = (loseMenuIndex - 1) % loseOptions.size();
		}break;
		case WIN_STATE:
		{
			winMenuIndex = (winMenuIndex - 1) % winOptions.size();
		}break;
		default:
			break;
		}//switch
	}// y button

	//pause game
	if (m_pController->GetButtonYToggle())
	{
		switch (state)
		{
			case PAUSE_STATE:
			{
				state = PLAY_STATE;
				break;
			}
			case WIN_STATE:
			{
				state = TITLE_STATE;
				break;
			}
			case PLAY_STATE:
			{
				state = PAUSE_STATE;
				break;
			}
			case LOSE_STATE:
			{
				state = PLAY_STATE;
				m_pAudio->stop();
				BeginGame();
			}break;
			default:
				break;
		}//switch on state
	}// if left shoulder toggle

	//interaction
	m_pPlayer->m_bInteract = false;
	if (m_pController->GetButtonXToggle() && state == PLAY_STATE) {
		m_pPlayer->m_bInteract = true;
	}
} //ControllerHandler

/// Ask the object manager to draw the game objects. RenderWorld
/// is notified of the start and end of the frame so
/// that it can let Direct3D do its pipelining jiggery-pokery.

void CGame::RenderFrame(){
  m_pRenderer->BeginFrame();
    m_pObjectManager->draw(); 
    m_pParticleEngine->Draw();

    // draw fps
    string s = to_string(m_pTimer->framerate()) + " fps";
    Vector2 pos(m_nWinWidth - 128.0f, 30.0f);
    m_pRenderer->DrawScreenText(s.c_str(), pos);

    //draw hud

    // calculate X and Y position of the hud
    float yPosition = m_pRenderer->GetCameraPos().y + (m_nWinHeight / 2) - (m_pRenderer->GetHeight(HP_SPRITE) / 2);
    float xPosition = m_pRenderer->GetCameraPos().x - (m_nWinWidth / 2) + (m_pRenderer->GetWidth(HP_SPRITE) / 2);
    Vector2 hpPOS(xPosition, yPosition);

    // draw hud sprite
    m_pRenderer->Draw(HP_SPRITE, hpPOS);
    
    // calculate text offsets
    Vector2 fastPos(30.0f, 0.0f);
    Vector2 fighterPos(86.0f, 0.0f);
    Vector2 shieldPos(149.0f, 0.0f);

		for (auto p : characters) {
      Vector2 text_pos; // position to render HP

      // set position based on sprite
      switch (p->m_nSpriteIndex) {
        case FAST_SPRITE: text_pos = fastPos; break;
        case FIGHTER_SPRITE: text_pos = fighterPos; break;
        case SHIELD_SPRITE: text_pos = shieldPos; break;
      }
      // render the text
			m_pRenderer->DrawScreenText(std::to_string(p->getHP()).c_str(), text_pos, Colors::Black);
		}
			 
    if (m_pPlayer) {
      const Vector2 player_pos = {m_pPlayer->m_vPos.x,  m_pPlayer->m_vPos.y + 32.0f};
      m_pRenderer->Draw(INDICATOR_SPRITE, player_pos);
    }
  m_pRenderer->EndFrame();
} //RenderFrame

/// Make the camera follow the player, but don't let it get
/// too close to the edge. Unless the world is smaller than
/// the window, in which case we center everything.

void CGame::FollowCamera(){
  Vector3 vCameraPos(m_pPlayer->GetPos()); //player position

  if(m_vWorldSize.x > m_nWinWidth){ //world wider than screen
    vCameraPos.x = max(vCameraPos.x, m_nWinWidth/2.0f); //stay away from the left edge
    vCameraPos.x = min(vCameraPos.x, m_vWorldSize.x - m_nWinWidth/2.0f);  //stay away from the right edge
  } //if
  else vCameraPos.x = m_vWorldSize.x/2.0f; //center horizontally.
  
  if(m_vWorldSize.y > m_nWinHeight){ //world higher than screen
    vCameraPos.y = max(vCameraPos.y, m_nWinHeight/2.0f);  //stay away from the bottom edge
    vCameraPos.y = min(vCameraPos.y, m_vWorldSize.y - m_nWinHeight/2.0f); //stay away from the top edge
  } //if
  else vCameraPos.y = m_vWorldSize.y/2.0f; //center vertically

  m_pRenderer->SetCameraPos(vCameraPos); //camera to player
} //FollowCamera

void CGame::nextLevel()
{
	m_nCurrentLevel = (m_nCurrentLevel+1) % NUM_LEVELS;
	BeginGame();
}

/// Handle keyboard input, move the game objects and render 
/// them in their new positions and orientations. Notify the 
/// audio player at the start of each frame so that it can 
/// prevent multiple copies of a sound from starting on the
/// same frame. Notify the timer of the start and end of the
/// frame so that it can calculate frame time. 
///
/// so we set dt to be 1/60 for the frame calculations.
/// we have an accumulator that measures the frame time
/// and processes physics and decreases by dt 
/// thus if it was longer dt it will run another physics calculation

void CGame::ProcessFrame(){
	switch (state) 
	{
		case PLAY_STATE:
		{
			KeyboardHandler(); //handle keyboard input
			ControllerHandler(); //handle controller input

			m_pAudio->BeginFrame(); //notify audio player that frame has begun
			m_pTimer->BeginFrame(); //notify timer that frame has begun

			const float dt = 1 / 60.0f; // timestep

			accumulator += m_pTimer->frametime() < 0.0f ? 0.0f : m_pTimer->frametime(); // make sure that is never negative and add it to the accumulator

			//while our accumulator still has a frame time left in it, do another step
			while (accumulator >= dt) 
			{

				m_pObjectManager->move(dt); //move all objects

				accumulator -= dt;
			}

			/*if (accumulator > 0.0f)
				m_pObjectManager->move(accumulator);*/

			FollowCamera(); //make camera follow player
			m_pParticleEngine->step(); //advance particle animation

			RenderFrame(); //render a frame of animation

			m_pTimer->EndFrame(); //notify timer that frame has ended
		}
		break;
		case TITLE_STATE:
		{
			KeyboardHandler(); //handle keyboard input
			ControllerHandler(); //handle controller input

			m_pAudio->BeginFrame(); //notify audio player that frame has begun
			m_pTimer->BeginFrame(); //notify timer that frame has begun

     if(state == TITLE_STATE) //play title music unless state has changed
       m_pAudio->loop(TITLE_SOUND);
     else
       m_pAudio->stop();

		 //make some sprite descriptors for the start button and the exit button
		 CSpriteDesc2D startDesc;
		 CSpriteDesc2D exitDesc;
		 startDesc.m_nSpriteIndex = START_BUTTON;
		 exitDesc.m_nSpriteIndex = EXIT_BUTTON;
		 startDesc.m_vPos = Vector2(m_vWinCenter.x, 293.0f);
		 exitDesc.m_vPos = Vector2(m_vWinCenter.x, 243.0f);

		 //based on which one is selected, make that sprite light up
		 if (menuOptions.at(mainMenuIndex) == START_BUTTON)
			 startDesc.m_nCurrentFrame = 1;
		 if (menuOptions.at(mainMenuIndex) == EXIT_BUTTON)
			 exitDesc.m_nCurrentFrame = 1;

			m_pRenderer->BeginFrame();
				m_pRenderer->Draw(TITLE_SCREEN, m_vWinCenter);
				m_pRenderer->Draw(startDesc);
				m_pRenderer->Draw(exitDesc);
        m_pRenderer->SetCameraPos((Vector3(m_vWinCenter.x,m_vWinCenter.y,0)));
			m_pRenderer->EndFrame();

			m_pTimer->EndFrame(); //notify timer that frame has ended
			break;
		}
    case PAUSE_STATE:
    {
      KeyboardHandler(); //handle keyboard input
      ControllerHandler(); //handle controller input

      m_pAudio->BeginFrame(); //notify audio player that frame has begun
      m_pTimer->BeginFrame(); //notify timer that frame has begun

			CSpriteDesc2D continueDesc;
			CSpriteDesc2D exitDesc;
			CSpriteDesc2D menuDesc;
			CSpriteDesc2D restartDesc;
			continueDesc.m_nSpriteIndex = CONTINUE_BUTTON;
			exitDesc.m_nSpriteIndex = EXIT_BUTTON;
			menuDesc.m_nSpriteIndex = MENU_BUTTON;
			restartDesc.m_nSpriteIndex = RESTART_SPRITE;
			continueDesc.m_vPos = Vector2(m_vWinCenter.x, 243.0f);
			menuDesc.m_vPos = Vector2(m_vWinCenter.x, 193.0f);
			exitDesc.m_vPos = Vector2(m_vWinCenter.x, 143.0f);
			restartDesc.m_vPos = Vector2(m_vWinCenter.x, 93.0f);

			if (pauseOptions.at(pauseMenuIndex) == CONTINUE_BUTTON)
				continueDesc.m_nCurrentFrame = 1;
			if (pauseOptions.at(pauseMenuIndex) == MENU_BUTTON)
				menuDesc.m_nCurrentFrame = 1;
			if (pauseOptions.at(pauseMenuIndex) == EXIT_BUTTON)
				exitDesc.m_nCurrentFrame = 1;
			if (pauseOptions.at(pauseMenuIndex) == RESTART_SPRITE)
				restartDesc.m_nCurrentFrame = 1;

      m_pRenderer->BeginFrame();
				m_pRenderer->Draw(PAUSE_SCREEN, m_vWinCenter);
				m_pRenderer->Draw(continueDesc);
				m_pRenderer->Draw(exitDesc);
				m_pRenderer->Draw(menuDesc);
				m_pRenderer->Draw(restartDesc);
				m_pRenderer->SetCameraPos((Vector3(m_vWinCenter.x, m_vWinCenter.y, 0)));
      m_pRenderer->EndFrame();

      m_pTimer->EndFrame(); //notify timer that frame has ended
      break;
    }
    case WIN_STATE:
    {
			if (m_nCurrentLevel + 1 == NUM_LEVELS)
			{
				KeyboardHandler(); //handle keyboard input
				ControllerHandler(); //handle controller input

				m_pAudio->BeginFrame(); //notify audio player that frame has begun
				m_pTimer->BeginFrame(); //notify timer that frame has begun

				CSpriteDesc2D exitDesc;
				CSpriteDesc2D menuDesc;
				exitDesc.m_nSpriteIndex = EXIT_BUTTON;
				menuDesc.m_nSpriteIndex = MENU_BUTTON;
				menuDesc.m_vPos = Vector2(m_vWinCenter.x, 193.0f);
				exitDesc.m_vPos = Vector2(m_vWinCenter.x, 143.0f);

				if (winOptions.at(winMenuIndex) == MENU_BUTTON)
					menuDesc.m_nCurrentFrame = 1;
				if (winOptions.at(winMenuIndex) == EXIT_BUTTON)
					exitDesc.m_nCurrentFrame = 1;

				m_pRenderer->BeginFrame();
				m_pRenderer->Draw(EXIT_SCREEN, m_vWinCenter);
				m_pRenderer->Draw(exitDesc);
				m_pRenderer->Draw(menuDesc);
				m_pRenderer->SetCameraPos((Vector3(m_vWinCenter.x, m_vWinCenter.y, 0)));
				m_pRenderer->EndFrame();

				m_pTimer->EndFrame(); //notify timer that frame has ended
			}
			else {
				// switch to next level
				state = PLAY_STATE;
				nextLevel();
			}
    }
		break;
		case LOSE_STATE:
		{
			KeyboardHandler(); //handle keyboard input
			//ControllerHandler(); //handle controller input

			m_pAudio->BeginFrame(); //notify audio player that frame has begun
			m_pTimer->BeginFrame(); //notify timer that frame has begun

      CSpriteDesc2D continueDesc;
      CSpriteDesc2D exitDesc;
      CSpriteDesc2D menuDesc;
      CSpriteDesc2D restartDesc;

      exitDesc.m_nSpriteIndex = EXIT_BUTTON;
      menuDesc.m_nSpriteIndex = MENU_BUTTON;
      restartDesc.m_nSpriteIndex = RESTART_SPRITE;

      menuDesc.m_vPos = Vector2(m_vWinCenter.x, 193.0f);
      exitDesc.m_vPos = Vector2(m_vWinCenter.x, 143.0f);
      restartDesc.m_vPos = Vector2(m_vWinCenter.x, 93.0f);


      if (loseOptions.at(loseMenuIndex) == MENU_BUTTON)
        menuDesc.m_nCurrentFrame = 1;
      if (loseOptions.at(loseMenuIndex) == EXIT_BUTTON)
        exitDesc.m_nCurrentFrame = 1;
      if (loseOptions.at(loseMenuIndex) == RESTART_SPRITE)
        restartDesc.m_nCurrentFrame = 1;

      m_pRenderer->BeginFrame();
      m_pRenderer->Draw(LOSE_SPRITE, m_vWinCenter);
      m_pRenderer->Draw(continueDesc);
      m_pRenderer->Draw(exitDesc);
      m_pRenderer->Draw(menuDesc);
      m_pRenderer->Draw(restartDesc);
      m_pRenderer->SetCameraPos((Vector3(m_vWinCenter.x, m_vWinCenter.y, 0)));
      m_pRenderer->EndFrame();

			m_pTimer->EndFrame(); //notify timer that frame has ended
			break;
		}
		default: break;
	}
} //ProcessFrame