#pragma once

#include "Object.h"
#include "ObjectManager.h"


/// \brief Door object. Inherited from CObject
/// 
/// contains state about if locked or if open
/// detects when a player is close enough to open a door
/// doesnt allow bullets to shoot through it
class Door :
	public CObject
{
	friend class CObjectManager;
private:
	bool m_bIsOpen; ///< is it open?
	bool m_bIsLocked; ///< is it locked?
	int m_nKey; ///< an int to denote which keys can unlock this door. Set to 0 for doors that are always unlocked.

public:
	Door(eSpriteType t, Vector2 p);
	Door(eSpriteType t, Vector2 p, int key);
  void animate(); ///< an overloaded animate for opening th door
	void queryPlayers();//if a player is in range of the door, set open to true
	bool getIsOpen();//returns true for open and false for closed
	void move(const float &);//slightly different than the CObject move. All it does is rotate and query the players
	void unlock();//unlock this door! called by a key
  int getKey() { return m_nKey; };
};

/// \brief class for the keys that unlock doors
///
///contains function to  unlock the doors that share its state
class Key :
	public CObject
{
	friend class CObjectManager;
private:
	int m_nKey; ///< its index
public:
	Key(eSpriteType t, Vector2 p);
	Key(eSpriteType t, Vector2 p, int k);
	void unlockDoors(); ///< look at all the doors and unlock the ones that i match
  void kill();///< commit seppuku
};