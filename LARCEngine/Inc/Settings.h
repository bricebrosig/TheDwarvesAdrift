/// \file Settings.h
/// \brief Interface for the settings class CSettings.

#pragma once

#include <Windows.h>

#include "tinyxml2.h"
#include "defines.h"

using namespace tinyxml2;

/// \brief Settings.
///
/// OS-dependent settings consisting of the app name,
/// the window width and height, and the position of the window's
/// center.

class CSettings{
  protected:
    static char m_szName[MAX_PATH]; ///< Name of this game. 
    
    static int m_nWinWidth; ///< Window width in pixels.
    static int m_nWinHeight; ///< Window height in pixels.
    static Vector2 m_vWinCenter; ///< Window center.
}; //CSettings

/// \brief Settings manager.
///
/// This class provides access to the game settings from
/// file gamesettings.xml. TinyXML2 is my preferred XML
/// file parser, not the one provided by Microsoft.

class CSettingsManager: public CSettings{
  private:
    tinyxml2::XMLDocument m_xmlDoc; ///< TinyXML2 document.

  protected:  
    static XMLElement* m_pXmlSettings; ///< Pointer to the settings tag in the XML settings file.   
    static float m_fAspectRatio; ///< Aspect ratio, width/ht. 

    void SetWinSize(int w, int h); ///< Set window size information.

  public: 
    void Load(); ///< Load game settings.
}; //CSettingsManager
