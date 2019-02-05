#include "Door.h"
#include "ComponentIncludes.h"

/// \brief default constructor "in case i need it"
Door::Door(eSpriteType t, Vector2 p) : CObject(t, p)
{
	m_bIsOpen = false;
	m_bIsLocked = false;
}

/// \brief constructor that does stuff
/// sets open to false since we assume that no one is near it at the start
/// if we are passed a 0 key we set locked to false
/// other wise we lock it
///set key to what was passed (0 or otherwise)
Door::Door(eSpriteType t, Vector2 p, int k) : CObject(t, p)
{
	m_bIsOpen = false;
	if (k == 0)
		m_bIsLocked = false;
	else
		m_bIsLocked = true;

	m_nKey = k;
}

/// \brief look at the players and their vectors to the door
///
/// if one is close enough itll open up
void Door::queryPlayers()
{
	Vector2 doorToPlayer;
	//m_bIsOpen = false;//set it to false and assume no one is close
	for (auto& p : characters)
	{
		doorToPlayer = m_vPos - p->GetPos();
		if (doorToPlayer.Length() < 2.5f*32.0f && !m_bIsLocked)
		{
			m_bIsOpen = true;//if they are close enough then open up
		}
	}
}

bool Door::getIsOpen()
{
	return m_bIsOpen;
}

/// \brief set locked to false
void Door::unlock()
{
	m_bIsLocked = false;
}

/// \brief animates the door sprite
///
/// Animates once per frame 
/// tints the door red if the door is locked
/// runs the animation is a player is close enough
void Door::animate() {
    // get the total number of frames for this sprite
    const size_t lastFrame = m_pRenderer->GetNumFrames(m_nSpriteIndex) - 1;
    const float dt = 250 * m_fFrameInterval / (1500 + fabs(m_vVelocity.x));

		//make some tints and then set them based on the lock state
		XMFLOAT4 lockedColor = XMFLOAT4(100.0f, 0.0f, 0.0f, 1.0f);
		XMFLOAT4 unlockedColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		if (m_bIsLocked) {
			m_f4Tint = lockedColor;
		}
		else {
			m_f4Tint = unlockedColor;
		}

		//animate
    if (m_pTimer->elapsed(m_fFrameTimer, dt)) {

      if (m_bIsOpen && m_nCurrentFrame != lastFrame) {
        // continue opening
        if (m_nCurrentFrame < lastFrame)
          m_nCurrentFrame++;
        else if (m_nCurrentFrame > lastFrame)
          m_nCurrentFrame = (unsigned)lastFrame;  // this shouldnt happen
      }
      else if (!m_bIsOpen && m_nCurrentFrame != 0) {
        // continue closing
        if (m_nCurrentFrame > 0)
          m_nCurrentFrame--;
        else if (m_nCurrentFrame < 0)
          m_nCurrentFrame = 0;  // this shouldnt happen
      }
    }
}

/// \brief an override of the CObject move
///	query the players and set open based on the length of the vectors to them
void Door::move(const float &t)
{
	Vector2 doorToPlayer;
	m_bIsOpen = false;//set it to false and assume no one is close

	for (auto& p : characters)
	{
		doorToPlayer = m_vPos - p->GetPos();
		if (abs(doorToPlayer.Length()) < 2.5f*32.0f && !m_bIsLocked)
			m_bIsOpen = true;//if they are close enough then open up
	}

  animate();
}

/***************key implementation***************/

/// \brief default constructor
/// set the key to zero... should need it but WHAT IF I DO
Key::Key(eSpriteType t, Vector2 p) : CObject(t, p)
{
	m_nKey = 0;
}

/// \brief constructor
/// \param t sprite index - goes to the CObject constructor
/// \param p position - also goes to the CObject constructor
/// \param k - the key of the key... make sense?
Key::Key(eSpriteType t, Vector2 p, int k) : CObject(t, p)
{
	m_nKey = k;
}

/// \brief unlock doors that match key
/// look through all the doors (got from object manager) and 
/// see if they match the key
/// if they do, unlock it and destroy this key
void Key::unlockDoors()
{
	for (auto d : m_pObjectManager->m_stdDoorList)
	{
		if (((Door*)d)->getKey() == m_nKey)
		{
			((Door*)d)->unlock();
      kill();
		}
	}
}

/// \brief shame really...
void Key::kill()
{
  m_bDead = true;
}