/// \file debug.cpp
/// \brief Code for debug manager class CDebugManager.
///
/// This file is made available under the GNU All-Permissive License.
/// Copying and distribution of this file, with or without
/// modification, are permitted in any medium without royalty
/// provided the copyright notice and this notice are preserved.
/// This file is offered as-is, without any warranty.

#include <string.h>
#include "debug.h"

CDebugManager::~CDebugManager(){ 
  SendToClient(END_DEBUG, m_szName);
} //destructor

/// Set the source code file and line number.
/// \param path Code file name that error came from
/// \param line Code line number that error came from

void CDebugManager::setsource(const char* path, int line){ 
  m_nLineNumber = line; 
  const char* p = strrchr(path, '\\'); //skip to the file name at the end of the full path
  m_strFileName = (p == nullptr)? path: (char*)(p + 1);
} //setsource

void CDebugManager::SendToClient(eDebugDataType t, const char* txt){
  COPYDATASTRUCT mydata; //data to be sent by WM_COPYDATA message

  mydata.dwData = t;
  mydata.cbData = (DWORD)strlen(txt) + 1;
  mydata.lpData = (PVOID)txt;

  HWND h = FindWindow("Ian Parberry's Debug Client", 0); //get a handle to the debug client

  if(h != NULL) //if it was found, that means it is running
    SendMessage(h, WM_COPYDATA, (WPARAM)m_Hwnd, (LPARAM)&mydata);
} //SendToClient

/// Debug printf function.
/// \param format printf style format string

void CDebugManager::printf(const char* format,...){ 
  string s = m_strFileName + "(" + to_string(m_nLineNumber) + "): ";

  char buffer[1024]; 
  memset(&buffer, 0x00, sizeof(buffer));

  va_list arglist;
  va_start(arglist,format);

  _vsnprintf_s((char*)(buffer + strlen(buffer)), 
    sizeof(buffer) - 1,
    sizeof(buffer) - strlen(buffer) - 1, 
    format, arglist);
  va_end(arglist);

  s += buffer;

  SendToClient(TEXT_DEBUG, s.c_str());
} //printf

/// Announce the start of the debug session.

void CDebugManager::Initialize(){
  SendToClient(START_DEBUG, m_szName);
} //Initialize

////////////////////////////////////////////////////////////////////////////
// End of CDebugManager functions.

CDebugManager g_cDebugManager; ///< The debug manager.

/// The real debug printf function.
/// \param format Printf style format string

void realDebugPrintf(const char *format, ...){
  static char buffer[1024];
  va_list ap;

  va_start(ap, format);
  _vsnprintf_s(buffer, sizeof(buffer), format, ap);
  va_end(ap);

  g_cDebugManager.printf("%s", buffer);
} //realDebugPrintf
