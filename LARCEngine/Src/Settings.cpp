/// \file Settings.cpp
/// \brief Code for the settings class CSettings.

#include "Settings.h"
#include "Abort.h"

//static member variables for CSettingsManager and CSettings.

XMLElement* CSettingsManager::m_pXmlSettings = nullptr;
float CSettingsManager::m_fAspectRatio = 1.0f; 

char CSettings::m_szName[];
int CSettings::m_nWinWidth = 0; 
int CSettings::m_nWinHeight = 0; 
Vector2 CSettings::m_vWinCenter = Vector2(0.0f);

///////////////////////////////////////////////////////////////////////////
// CSettingsManager functions

/// Load settings from a fixed settings file, gamesettings.xml.

void CSettingsManager::Load(){    
  //load settings file
  if(m_xmlDoc.LoadFile("Media\\xml\\gamesettings.xml"))
    ABORT("Cannot load settings file gamesettings.xml.");

  //get settings tag

  XMLElement* pSettingsTag = m_xmlDoc.FirstChildElement("settings");
  m_pXmlSettings = pSettingsTag; //settings tag

  if(pSettingsTag == nullptr) //abort if tag not found
    ABORT("Cannot find <settings> tag in gamesettings.xml.");

  //get game name

  XMLElement* pGameTag = pSettingsTag->FirstChildElement("game"); 

  if(pGameTag == nullptr) //empty name
    m_szName[0] = '\0';

  else{ //get name from name tag
    const char* szName = pGameTag->Attribute("name");
    const int n = (int)strlen(szName);

    strncpy_s(m_szName, MAX_PATH, szName, n); 
  } //else

  //get renderer settings

  XMLElement* pRendererTag = pSettingsTag->FirstChildElement("renderer"); //renderer tag

  if(pRendererTag != nullptr){ //read renderer tag attributes
    const int w = pRendererTag->IntAttribute("width");
    const int h = pRendererTag->IntAttribute("height");

    SetWinSize(w, h); //set the window size
  } //if
} //Load

/// Store the window's width and height, and compute and
/// store the aspect ratio and the position of the window
/// center (ie. half its width and half its height).
/// \param w Window client area width in pixels.
/// \param h Window client area height in pixels.

void CSettingsManager::SetWinSize(int w, int h){
  m_nWinWidth = w;
  m_nWinHeight = h;
  m_fAspectRatio = (float)w/h;
  m_vWinCenter = Vector2(w/2.0f, h/2.0f);
} //SetWinSize


