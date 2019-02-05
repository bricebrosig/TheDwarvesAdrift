#pragma once

#include "Object.h"

/// \brief class for the enemies
///
/// they contain state about whether they are patrolling or attacking
/// they detect when enemies are near
/// if an enemy is near it will attack it
/// otherwise it bounces between its patrol points
class EvilNPC :
	public CObject
{
	friend class CObjectManager;
private:
	Vector2 patrolCoords; ///< where it can walk between when patrolling
	State state; ///< attacking or patrolling
	float attackRange; ///< how far can i detect enemies

public:
	EvilNPC(State st, Vector2 coords, float range, eSpriteType t, const Vector2& p); ///< constructor
	bool checkForPlayer(); ///< is a player close to me
  void CollisionResponse(const BoundingBox& b); ///< how i respond to my patrol points and regular collision
	void move(const float &); ///< overloaded move 
	int getState(); ///< return state 
	Vector2 getPatrolCoords(); ///< return patrol coords
	void setState(State); ///< change our state
	void setPatrolCoords(Vector2); ///< change the patrol coords
};

