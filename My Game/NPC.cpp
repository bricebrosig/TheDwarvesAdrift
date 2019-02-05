#include "NPC.h"
#include "ComponentIncludes.h"

EvilNPC::EvilNPC(State st, Vector2 coords, float range, eSpriteType t, const Vector2& p) : CObject(t,p)
{
	state = st;
	patrolCoords = coords;
	attackRange = range;
}

bool EvilNPC::checkForPlayer()
{
  return false;
}

/// \ brief move our enemie npc
///
/// we determine which way we are facing and 
/// apply an x velocity based on that amount
/// also calls the CObject animate function that 
/// makes us look like we are walking
/// make the new bounding boxes
/// \param timestep
void EvilNPC::move(const float & t) 
{
	//const float t = m_pTimer->frametime();
	//const float t = 1 / 60.0f;

	m_vVelocity -= m_fGravity * t*Vector2::UnitY; // do gravity
	m_vPos += 64.0f*t*m_vVelocity; // move that much

  if(isFacing(RIGHT))
    m_vVelocity.x = 0.5f;
  else
    m_vVelocity.x = -0.5f;

	m_vPos += m_vVelocity * t; //move by that much

  // dont animate if not moving
  if(m_vVelocity.x != 0.0f)
	  animate(RUN_ANIM);

	//update bounding shapes

	const Vector2 topleft(-m_vRadius.x, m_vRadius.y);
	const Vector2 bottomrt(m_vRadius.x, -m_vRadius.y);
	BoundingBox::CreateFromPoints(m_Aabb, topleft + m_vPos, bottomrt + m_vPos);

	m_Sphere.Center = (Vector3)m_vPos;
}

/// Perform the appropriate response to a collision.
/// a modified collision response that takes into account which way we are facing and if we are hitting 
/// one of the patrol points
/// \param b Axially aligned bounding box for the object it collided with.

void EvilNPC::CollisionResponse(const BoundingBox& b) {
  float w, h;
  m_pRenderer->GetSize(m_nSpriteIndex, w, h);
  w /= 2; h /= 2;

  const float top = b.Center.y + b.Extents.y;
  const float bottom = b.Center.y - b.Extents.y;

  const float left = b.Center.x - b.Extents.x;
  const float right = b.Center.x + b.Extents.x;

  // standard Y collision
  if (m_vPos.y > top && m_vPos.y < top + h) {
    m_vPos.y = top + h;
    m_vVelocity.y = 0;
  } //if
 

  // move back, then if patrolling reverse direction
  if (m_vPos.x < left && m_vPos.x > left - w) {
    m_vPos.x = left - w - 0.5f;
    if (state == PATROL) { // turn around
      m_vVelocity.x = -1.0f * m_vVelocity.x;
      // change facing direction
      if (m_nFacing == RIGHT)
        m_nFacing = LEFT;
      else
        m_nFacing = RIGHT;
    }
    else {
      m_vVelocity.x = 0;
    }
    
  } //else if

  else if (m_vPos.x > right && m_vPos.x < right + w) {
    m_vPos.x = right + w + 0.5f;
    if (state == PATROL) { // turn around
      m_vVelocity.x = -1.0f * m_vVelocity.x;
      // change facing direction
      if (m_nFacing == RIGHT)
        m_nFacing = LEFT;
      else
        m_nFacing = RIGHT;
    }
    else {
      m_vVelocity.x = 0;
    }

  } //else if
} //CollisionResponse

//sets and gets
int EvilNPC::getState() { return state; }
Vector2 EvilNPC::getPatrolCoords() { return patrolCoords; }
void EvilNPC::setState(State s) { state = s; }
void EvilNPC::setPatrolCoords(Vector2 c) { patrolCoords = c; }