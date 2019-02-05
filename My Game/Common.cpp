/// \file Common.cpp
/// \brief Code for the class CCommon.
///
/// This file contains declarations and initial values
/// for CCommon's static member variables.

#include "Common.h"

CRenderer* CCommon::m_pRenderer = nullptr;
CObjectManager* CCommon::m_pObjectManager = nullptr;
CParticleEngine2D* CCommon::m_pParticleEngine = nullptr;

Vector2 CCommon::m_vWorldSize = Vector2::Zero;
bool CCommon::m_bDrawAABBs = false;
CObject* CCommon::m_pPlayer = nullptr;
CObject* CCommon::m_pShieldPlayer = nullptr;
float CCommon::m_fGravity = 20.0f;
float CCommon::elevatorVel = 0.0f;
gameState CCommon::state = TITLE_STATE;
int CCommon::whichPlayer = 2;
vector<CObject*> CCommon::characters = vector<CObject*>(3);


