/// \file debug.h
/// \brief Debug definitions.
///
/// Interface for the debug manager class CDebugManager, and definition
/// of the DEBUGPRINTF macro. This macro will be defined if you are compiling
/// in Debug mode, otherwise it will be aliased to nil, naught, nothing,
/// not even a shadow. Of course, if you want to use it in Release mode,
/// you will have to move the "#define DEBUG_ON" outside of the "#ifdef".

#pragma once

#include <windows.h>
#include <stdio.h>

#include "Settings.h"
#include "WindowDesc.h"

#include <string>

using namespace std;

/// \brief Debug message type.
///
/// Type of data to be sent to debug client.

enum eDebugDataType{
  START_DEBUG, TEXT_DEBUG, END_DEBUG
}; //eDebugDataType

/// \brief The debug manager.
///
/// The debug manager is designed to be accessed using the macro DEBUGPRINTF.

class CDebugManager: 
  public CSettingsManager,
  public CWindowDesc{ 

  private:
    string m_strBuffer; ///< Output buffer.

    string m_strFileName; ///< File that debug message is from.
    int m_nLineNumber; ///< Line that debug message is from.

    void SendToClient(eDebugDataType t, _In_ const char* txt); ///< Send debug data to client.

  public:
    ~CDebugManager(); ///< Destructor.

    void Initialize(); ///<Initialize.

    void printf(_In_ const char* format,...); ///< Debug printf.
    void setsource(_In_ const char* path, int line); ///< Set file and line number.
}; //CDebugManager
 
extern CDebugManager g_cDebugManager;

/// \brief Real debug printf function.
/// Normally we will call this function using the DEBUGPRINTF macro.

void realDebugPrintf(const char *fmt, ...);


