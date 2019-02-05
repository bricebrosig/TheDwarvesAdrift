/// \file Object.cpp
/// \brief Code for the game object class CObject.

#include "Object.h"
#include "ComponentIncludes.h"
#include "GameDefines.h"
#include "Particle.h"
#include "ParticleEngine.h"

#include "DebugPrintf.h"

/// Create and initialize an object given its sprite type
/// and initial position.
/// \param t Type of sprite.
/// \param p Initial position of object.

CObject::CObject(eSpriteType t, const Vector2& p){ 
  m_nSpriteIndex = t;
  m_vPos = p;

  m_pRenderer->GetSize(t, m_vRadius.x, m_vRadius.y);
  m_vRadius *= 0.5f;
  
  const Vector2 topleft(-m_vRadius.x, m_vRadius.y);
  const Vector2 bottomrt(m_vRadius.x, -m_vRadius.y);
  BoundingBox::CreateFromPoints(m_Aabb, topleft + m_vPos, bottomrt + m_vPos);

  m_Sphere.Radius = max(m_vRadius.x, m_vRadius.y);
  m_Sphere.Center = (Vector3)m_vPos;

  m_fGunTimer = m_pTimer->time();
  m_fBirthTime = m_pTimer->time();

  // swords need to die
  if (t == SWORD_SPRITE) {
    m_fLifeTime = m_pTimer->frametime()*10;
  }
} //constructor

/// Move and update all bounding shapes.
/// The player object gets moved by the controller, everything
/// else moves an amount that depends on its velocity and the
/// frame time.
/// Switches on the sprite type and does different moves based off of that

void CObject::move(const float & t){
  //const float t = m_pTimer->frametime();
	//const float t = 1 / 60.0f;

  // move based on sprite
  switch (m_nSpriteIndex) {
    // if sword an
		case KEY_SPRITE: { //have gravity
			m_vVelocity -= m_fGravity * t*Vector2::UnitY;
			m_vPos += 64.0f*t*m_vVelocity;
		}break;	
		case SHIELDUP_SPRITE:
		{
			// if the shield player is dead then we should commit suicide and not do anything else
			if (m_pShieldPlayer == nullptr)
				this->kill();
			else if(m_bShieldUp) // if it up we want to rotate it and have it above us
			{
				//put it at the shiled player
				m_vVelocity = m_pShieldPlayer->GetVelocity();
				m_vPos = m_pShieldPlayer->GetPos();

				m_vPos.y += 16.0f; // move it above shield man

				m_fRoll = XM_PIDIV2; // spin it 90 degrees
			}
			else //if its down we want it up and down and in front of us
			{
				m_vVelocity = m_pShieldPlayer->GetVelocity();
				m_vPos = m_pShieldPlayer->GetPos();

				m_fRoll = 0.0f; // spin it back to normal

				if (m_pShieldPlayer->isFacing(RIGHT))
					m_vPos.x += 22.0f;
				else
					m_vPos.x -= 22.0f;
			}
		}break;
    case SWORD_SPRITE:
    {
			//our sword needs to be where the fighter man is but offset slightly
      if (m_pPlayer->m_nSpriteIndex == FIGHTER_SPRITE) {
        m_vVelocity = m_pPlayer->GetVelocity();
        m_vPos = m_pPlayer->GetPos();

        m_vPos.y -= 4.0f; // move the sword slightly below midpoint

        if (m_pPlayer->isFacing(RIGHT)) {
          m_vPos.x += 24.0f;
          m_nCurrentFrame = 0; // right facing sword
        }
        else {
          m_vPos.x -= 24.0f;
          m_nCurrentFrame = 1; // left facing sword
        }
          
      }
    } break;
    case FIGHTER_SPRITE:
    case SHIELD_SPRITE:
		case FAST_SPRITE:
    {
			float delta;

			if (m_nSpriteIndex == FAST_SPRITE)
				delta = 250.0f*t;
			else
				delta = 200.0f*t;

      // y movement
      if (m_bOnLadder) {
				// Reset doublejump
				m_nJumps = GROUND;

        // lets not have any velocity
        m_vVelocity = Vector2::Zero;

				//when we are on ladders
        if (m_bClimbUp) 
          m_vPos += delta * Vector2::UnitY;
        else if (m_bClimbDown)
          m_vPos -= delta * Vector2::UnitY;

      }
      else if (m_bOnElevator) {
				m_nJumps = GROUND;
        m_vVelocity.y = t*elevatorVel;
        m_vPos += m_vVelocity * t* 60.5f;
      }
      else {
        //lets fall with gravity
				if(m_nSpriteIndex == SHIELD_SPRITE && m_bShieldUp)
					m_vVelocity = -150.0f * t*Vector2::UnitY;
				else
					m_vVelocity -= m_fGravity * t*Vector2::UnitY;

        m_vPos += 64.0f*t*m_vVelocity;
      }

      // strafe is the same speed on/off the ladder
      if (m_bStrafeRight) {
        animate(RUN_ANIM);
        m_vPos += delta * Vector2::UnitX;
      }
      else if (m_bStrafeLeft) {
        animate(RUN_ANIM);
        m_vPos -= delta * Vector2::UnitX;
      }
        


      m_bStrafeLeft = m_bStrafeRight = m_bStrafeBack = m_bClimbUp = m_bClimbDown = false; //set em back to false after we move up
			
    }break;//case fighter/fast/shield sprite
    
    default: m_vPos += m_vVelocity * t;
  }
  //update bounding shapes


  const Vector2 topleft(-m_vRadius.x, m_vRadius.y);
  const Vector2 bottomrt(m_vRadius.x, -m_vRadius.y);
  BoundingBox::CreateFromPoints(m_Aabb, topleft + m_vPos, bottomrt + m_vPos);

  m_Sphere.Center = (Vector3)m_vPos;
} //move

/// Make the object jump straight up. Only works when the player's
/// vertical speed is zero. Theoretically you can double-jump
/// (and more) if you hit it right at the top of the arc, but
/// iy's tricky.

void CObject::Jump(){
	if (m_vVelocity.y == 0) {
		m_vVelocity.y = 10.5f;
		m_nJumps = SINGLEJUMP;
	}
	else if (m_nJumps == SINGLEJUMP) {
		m_vVelocity.y = 7.5f;
		m_nJumps = DOUBLEJUMP;
	}
		
} //Jump

/// \brief when we hit a trap we want to jump a bit
void CObject::TrapReaction() {
		m_vVelocity.y = 7.5f;
		m_nJumps = SINGLEJUMP;
} //Jump

/// \ when we climb up a ladder we oughta set that bool to true so move knows what to do
void CObject::climbUp() {
	m_bClimbUp = true;
}

/// \brief when we climb down a ladder we ougtha do the same
void CObject::climbDown() {
	m_bClimbDown = true;
}

/// return that bool
bool CObject::isOnLadder(){
	return m_bOnLadder;
}

/// Perform the appropriate response to a collision.
/// \param b Axially aligned bounding box for the object it collided with.

void CObject::CollisionResponse(const BoundingBox& b){
  float w, h;
  m_pRenderer->GetSize(m_nSpriteIndex, w, h);
  w /= 2; h /= 2;
  
  const float top = b.Center.y + b.Extents.y;
  const float bottom = b.Center.y - b.Extents.y;

  const float left = b.Center.x - b.Extents.x;
  const float right = b.Center.x + b.Extents.x;

  if(m_vPos.y > top && m_vPos.y < top + h){ //collide with top
    m_vPos.y = top + h;
		m_vVelocity.y = 0;
  } //if

  else if(m_vPos.y < bottom && m_vPos.y > bottom - h){
    m_vPos.y = bottom - h - 0.5f;
    m_vVelocity.y = 0;
  } //else if

  else if(m_vPos.x < left && m_vPos.x > left - w && m_vPos.y - h < top){//if above bb dont collide from left or right
    m_vPos.x = left - w - 0.5f;
    m_vVelocity.x = 0;
  } //else if

  else if(m_vPos.x > right && m_vPos.x < right + w && m_vPos.y - h < top){//same^
    m_vPos.x = right + w + 0.5f;
    m_vVelocity.x = 0;
  } //else if
} //CollisionResponse

/// \ moves us where we should be when we collide with an elevator
void CObject::elevatorResponse(const BoundingBox& b) {
  float w, h;
  m_pRenderer->GetSize(m_nSpriteIndex, w, h);
  h /= 2;

  const float top = b.Center.y + b.Extents.y;

  m_vPos.y = top + h;
  m_vVelocity.y = 0;
}


/// Set the strafe left flag.

void CObject::StrafeLeft(){
  m_bStrafeLeft = true;
} //StrafeLeft

/// Set the strafe right flag.

void CObject::StrafeRight(){
  m_bStrafeRight = true;
} //StrafeRight

/// Set the strafe back flag.

void CObject::StrafeBack(){
  m_bStrafeBack = true;
} //StrafeBack

void CObject::turn(facingDirection dir){
	m_nFacing = dir;
}

bool CObject::isFacing(facingDirection dir){
	if (m_nFacing == dir)
		return true;
	return false;
}

void CObject::animate(animationType anime) {//im hilarious
  switch (anime)
  {
  case RUN_ANIM:
  {
    // get the total number of frames for this sprite
    const size_t nFrameCount = m_pRenderer->GetNumFrames(m_nSpriteIndex);
    const float dt = 2000 * m_fFrameInterval / (1500 + fabs(m_vVelocity.x));

    if (nFrameCount > 1 && m_pTimer->elapsed(m_fFrameTimer, dt)) {
      // get the current frame
      // its modulo the first half of the character sequence
      m_nCurrentFrame = (m_nCurrentFrame + 1) % (nFrameCount / 2);

      // use second half of animation cycle
      if (isFacing(LEFT)) {
        // move to the second half
        m_nCurrentFrame += (unsigned)(nFrameCount / 2);
      }
    }

  }
  case JUMP_ANIM: break;
  case ATTACK_ANIM: break;
  case DIE_ANIM: break;
  case WALL_BREAK:
  {
    const size_t nFrameCount = m_pRenderer->GetNumFrames(m_nSpriteIndex);
    const float dt = 1000 * m_fFrameInterval / (1500 + fabs(m_vVelocity.x));

    if (nFrameCount > 1 && m_pTimer->elapsed(m_fFrameTimer, dt))
      m_nCurrentFrame = (m_nCurrentFrame + 1) % nFrameCount;

    break;
  }break;
  default: break;
  }

  // look different if immune to damage
  if ((m_pTimer->time() - m_fLastDamageTime) < m_fDamageTimer) {
    m_f4Tint = XMFLOAT4(Colors::LightPink);
  }
  else {
    m_f4Tint = XMFLOAT4(1, 1, 1, 1);
  }
}



/// Perform a death particle effect to mark the death of an object.

void CObject::DeathFX(){
  CParticleDesc2D d;
  d.m_nSpriteIndex = SMOKE_SPRITE;
  d.m_vPos = m_vPos;
      
  d.m_fLifeSpan = 0.5f;
  d.m_fMaxScale = 0.5f;
  d.m_fScaleInFrac = 0.2f;
  d.m_fFadeOutFrac = 0.8f;
  d.m_fScaleOutFrac = d.m_fFadeOutFrac;

  m_pParticleEngine->create(d);

  if (m_nSpriteIndex == FIGHTER_SPRITE || m_nSpriteIndex == FAST_SPRITE || m_nSpriteIndex == SHIELD_SPRITE) {
    m_pAudio->play(DEATH_SOUND);
  }
} //DeathFX

/// Kill an object by marking its "is dead" flag. The object
/// will get deleted later at the appropriate time.

void CObject::kill(){
  m_bDead = true;
  DeathFX();
} //kill

bool CObject::tooOld()
{
  return (m_pTimer->time() - m_fBirthTime) > m_fLifeTime;
}

/// Reader function for the "is dead" flag.
/// \return true if marked as being dead, that is, ready for disposal.

bool CObject::IsDead(){
  return m_bDead;
} //IsDead

void CObject::damage(int hpLost){

	if ((m_pTimer->time() - m_fLastDamageTime) > m_fDamageTimer) {
		m_hp -= hpLost;
		m_fLastDamageTime = m_pTimer->time();
    m_pAudio->vary(OW_SOUND);
	}

	if (m_hp <= 0) {
    kill();

    // switch characters if current character dies
    if (m_pPlayer == this) {
      // switch characters or game over
      if (characters.size() > 1) {
        whichPlayer = (whichPlayer + 1) % characters.size();
        m_pPlayer = characters.at(whichPlayer);
      }
      else
        //game over
        state = LOSE_STATE;
    }
	}
}

/// Reader function for speed.
/// \return Speed.

float CObject::GetSpeed(){
  return m_fSpeed;
} //GetSpeed

/// Set the object's speed, assuming that the object
/// moves according to its speed and view vector.
/// \param speed Speed.

void CObject::SetSpeed(float speed){
  m_fSpeed = speed;
} //SetVelocity

/// Set the object's rotational speed in revolutions per second.
/// \param speed Rotational speed in RPS.

void CObject::SetRotSpeed(float speed){
  m_fRotSpeed = speed;
} //Rotate

/// Reader function for the AABB.
/// \return The AABB.

const BoundingBox& CObject::GetBoundingBox(){
  return m_Aabb;
} //GetBoundingBox

/// Reader function for the bounding sphere.
/// \return The bounding sphere.

const BoundingSphere& CObject::GetBoundingSphere(){
  return m_Sphere;
} //GetBoundingSphere

/// Reader function for position.
/// \return Position.

const Vector2& CObject::GetPos(){
  return m_vPos;
} //GetPos

/// Get the view vector.
/// \return The view vector.

Vector2 CObject::GetViewVector(){
  return Vector2(-sin(m_fRoll), cos(m_fRoll));
} //GetViewVector

/// Reader function for the orientation. A 2D object's
/// orientation is its roll amount in 3D space.
/// \return The view vector.

float CObject::GetOrientation(){
  return m_fRoll;
} //GetOrientation

/// Writer function for the orientation. A 2D object's
/// orientation is its roll amount in 3D space.
/// \param angle The new orientation.

void CObject::SetOrientation(float angle){
  m_fRoll = angle;
} //SetOrientation

/// Reader function for velocity.
/// \return Velocity.

const Vector2& CObject::GetVelocity(){
  return m_vVelocity;
} //GetVel

/// Writer function for velocity.
/// \param v Velocity.

void CObject::SetVelocity(const Vector2& v){
  m_vVelocity = v;
} //SetVel


