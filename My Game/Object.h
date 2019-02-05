/// \file Object.h
/// \brief Interface for the game object class CObject.

#pragma once

#include "GameDefines.h"
#include "Renderer.h"
#include "Common.h"
#include "Component.h"
#include "SpriteDesc.h"

enum State { PATROL, ATTACK };
enum JumpState {GROUND, SINGLEJUMP, DOUBLEJUMP};

/// \brief The game object. 
///
/// CObject is the abstract representation of an object.

class CObject:
  public CCommon,
  public CComponent,
  public CSpriteDesc2D
{
  friend class CObjectManager;
  protected:
    BoundingSphere m_Sphere; ///< Bounding sphere.
    BoundingBox m_Aabb; ///< Axially aligned bounding box.

		//ripped from neds turkey farms
		float m_fFrameTimer = 0.0f; ///< Last time the frame was changed.
		float m_fFrameInterval = 0.1f; ///< Interval between frames.
    float m_fBirthTime = 0.0f; ///< when we came into this world
    float m_fLifeTime = 0.0f; ///< when we ought to leave this world
		
		float m_fDamageTimer = 0.75f; ///< how long before we can take more damage
		float m_fLastDamageTime = 0.0f; ///< when the last damage was taken

    float m_fSpeed = 0.0f; ///< Speed.
    float m_fRotSpeed = 0.0f; ///< Rotational speed.
    Vector2 m_vVelocity = Vector2::Zero; ///< Velocity.
    bool m_bDead = false; ///< Is dead or not.
    
    bool m_bStrafeLeft = false; ///< Strafe left.
    bool m_bStrafeRight = false; ///< Strafe right.
    bool m_bStrafeBack = false; ///< Strafe back.
		bool m_bClimbUp = false; ///< climbing up
		bool m_bClimbDown = false; ///< climbing down

    int m_hp = 3; // player hp

		JumpState m_nJumps = GROUND; ///< state of the double jump

		bool m_bOnLadder = false;
    bool m_bOnElevator = false;

		facingDirection m_nFacing = RIGHT;
    Vector2 m_vRadius = Vector2::Zero; ///< Half width and height of object sprite.

    //float m_fGunTimer = 0.0f; ///< Gun fire timer.

		bool m_bShieldUp = false; ///< state of the shield
  public:

		float m_fGunTimer = 0.0f; ///< Gun fire timer.

    CObject(eSpriteType t, const Vector2& p); ///< Constructor.

    virtual void move(const float&); ///< Move object.
    void Jump(); ///< Jump.
		void climbUp(); ///< this makes object move up a ladder
		void climbDown(); ///< this makes object go down a ladder
		bool isOnLadder(); ///< query if they are touching a ladder tile
		bool m_bInteract = false; ///< a bool to know if the player is interacting

    virtual void kill(); ///< Kill me.
    bool IsDead(); ///< Query whether dead.
    virtual void DeathFX(); ///< Death special effects.
    virtual void CollisionResponse(const BoundingBox& b); ///< Collision response.
    virtual void elevatorResponse(const BoundingBox& b); ///< collision response for an elevator

    void SetSpeed(float speed); ///< Set speed.
    float GetSpeed(); ///< Get speed.
    void SetRotSpeed(float speed); ///< Set rotational velocity.

    Vector2 GetViewVector(); //Get view vector.
    float GetOrientation(); ///< Get orientation.
    void SetOrientation(float angle); ///< Set orientation.
    
    const Vector2& GetVelocity(); ///< Get orientation.
    void SetVelocity(const Vector2& v); ///< Set orientation.
    
    int getHP() { return m_hp; };

    void StrafeLeft(); ///< Strafe left.
    void StrafeRight(); ///< Strafe right.
    void StrafeBack(); ///< Strafe back.
		void animate(animationType);
		void turn(facingDirection dir);
		bool isFacing(facingDirection dir);
		float getGunTimer() { return m_fGunTimer; }
    
    const BoundingBox& GetBoundingBox(); ///< Get AABB.
    const BoundingSphere& GetBoundingSphere(); ///< Get bounding sphere.
    const Vector2& GetPos(); ///< Get position.
		void TrapReaction(); ///< jump a bit when hitting a trap

    bool tooOld(); ///< timer for when the object has lived long enough
		void damage(int); ///< damage a player by a certain amount

		virtual void setState(State st) {} ///< this is a virtual function defined in EvilNPC
}; //CObject