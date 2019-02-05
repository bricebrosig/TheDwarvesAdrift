/// \file Main.cpp 
/// \brief Every program has to have a main.
///
/// Instantiate an instance of CWindow from the Engine project,
/// which will handle Windows-specific things. This file should
/// almost always be the only place that nasty details of Windows
/// programming should be allowed to intrude on our nice
/// clean game code, which is instantiated in an instance of
/// CGame. Code for the obligatory Windows function WinMain
/// is provided here. It calls CWindow's DefaultWinMain function
/// to take care of Windows things, calls CGame's InitGame
/// function to initialize the game, and then calls CGame's
/// ProcessFrame function as often as Windows will let it.

#include "Defines.h"
#include "Game.h"
#include "Window.h"

#include "DebugPrintf.h"

#ifdef _DEBUG
  //#include <vld.h> //Visual Leak Detector from http://vld.codeplex.com/
#endif

static CWindow g_cWindow; ///< The window class.
static CGame g_cGame; ///< The game class.

/// \brief wWinmain is the Windows equivalent of main(), wide character version.  
///
/// This is the main entry point for this application. 
/// It calls DefaultWinMain from the Engine project,
/// calls CGame's InitGame function to initialize
/// the game, then enters the required Windows message
/// loop.  The latter performs Windows' mandatory message processing
/// and calls CGame's ProcessFrame function as often as it can.
/// Dont mess with this function unless you grok Windows programming.
/// \param hInstance Handle to the current instance of this application.
/// \param hPrevInstance Unused.
/// \param lpCmdLine Unused.
/// \param nCmdShow Nonzero if window is to be shown.
/// \return 0 If this application terminates correctly, otherwise an error code.

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
  _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  if(!g_cWindow.WinMain(hInstance))return 1; 

  Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
  if(FAILED(initialize))return 1;
  
  /**** BEGIN <DO NOT MESS WITH THIS CODE ZONE> ****/
  #ifdef USE_DEBUGPRINTF
    g_cDebugManager.Initialize();
  #endif //USE_DEBUGPRINTF
  /**** END <DO NOT MESS WITH THIS CODE ZONE> ****/

  g_cGame.Initialize(); //initialize the game
  
  //message loop

  MSG msg = {0}; //current message

  while(msg.message != WM_QUIT){
    if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){ 
      TranslateMessage(&msg);
      DispatchMessage(&msg); 
    } //if

    else g_cGame.ProcessFrame();
  } //while

  //clean up and exit

  CoUninitialize();

  return (int)msg.wParam;
} //wWinMain