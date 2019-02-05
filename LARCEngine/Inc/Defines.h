/// \file defines.h
/// \brief Essential game engine defines and includes. 
///
/// I can't take any credit for this because it's copied
/// from various Microsoft demos.

#pragma once

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0A00
#include <SDKDDKVer.h>

#define NOMINMAX //use the C++ standard templated min/max

//DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

#define NOMCX
#define NOSERVICE
#define NOHELP //WinHelp is deprecated

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl/client.h>
#include <wrl/event.h>

#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <d3d12.h>
#include <DirectXMath.h>

#include "SimpleMath.h"

using namespace DirectX;
using namespace SimpleMath;

