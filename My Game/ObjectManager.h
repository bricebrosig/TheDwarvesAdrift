/// \file ObjectManager.h
/// \brief Interface for the object manager CObjectManager.

#pragma once

#include <list>

#include "Object.h"

#include "Component.h"
#include "Common.h"
#include "Settings.h"
#include "TileManager.h"
#include "NPC.h"
#include "Door.h"
#include "Elevator.h"
#include "Traps.h"
#include "GunTurret.h"

using namespace std;

/// \brief The object manager.
///
/// A collection of all of the game objects.

class CObjectManager: 
  public CComponent, 
  public CCommon, 
  public CSettings{
	friend class Key;

  private:
    list<CObject*> m_stdObjectList; ///< Object list.
		list<CObject*> m_stdDoorList; ///< list of all the doors
		vector<CObject*> m_vTrapList; ///< vector l the traps
		vector<CObject*> m_vSwitchList; ///< list of all the switches
		vector<CObject*> m_stdPlatformList; ///< vector of all the platforms 
		list<CObject*> m_stdEndPointList; ///< list of all the endpoints
    list<CObject*> m_stdStopPointList; ///< list of all the endpoints
		vector<CObject*> m_vTeleporterList; ///< vector of all the teleporters 
		vector<CObject*> m_vBreakableWallList; ///< vector of all the destructable walls
		vector<CObject*> m_vTeleportSpawns; ///< vector of all the spawns for teleporters
		vector<CObject*> m_vKeys; ///< a list of all the key objects
		vector<CObject*> m_vEnemies; ///< a vector of all the enemies
    vector<CObject*> m_vGuns; ///< a vector of all the enemies
    CTileManager* m_pTileManager = nullptr; ///< Pointer to tile manager.
    CObject* m_pSwordPointer = nullptr; ///< a pointer to the sword that our attack character holds
		CObject* m_pShieldPointer = nullptr; ///< a pointer to the shield that our defense character holds

    void BroadPhase(); ///< Broad phase collision detection and response.
    void NarrowPhase(CObject* p0, CObject* p1); ///< Narrow phase collision detection and response.

    void CullDeadObjects(); ///< Cull dead objects.

  public:
    CObjectManager(); ///< Constructor.
    ~CObjectManager(); ///< Destructor.

    CObject* create(eSpriteType t, const Vector2& v); ///< Create new object.
		CObject* create(eSpriteType t, const Vector2& v, int i); ///< switches and traps have an extra param that is their key

		CObject* isTouchingElevator(); ///< check if our player is currently colliding with an elevator

    void clear(); ///< Reset to initial conditions.
    void move(const float &); ///< Move all objects.
    void draw(); ///< Draw all objects.

    void FireGun(CObject* p, eSpriteType bullet); ///< Fire object's gun.
    void FireGun(CObject* p, eSpriteType bullet, CObject* c); ///< Fire object's gun.
    void swingSword(); ///< have the attack player swing his sword
		void flipShield(); ///< toggle between shield man having the shield up or in front

    void LoadMap(int level); ///< Load a level.

		vector<CObject*> getVecTraps();
		vector<CObject*> getVecSwitches();
}; //CObjectManager