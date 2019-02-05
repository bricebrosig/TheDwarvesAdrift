#include "Elevator.h"

/// \constructor
///sets states and what
Elevator::Elevator(platformState ps, bool w, eSpriteType t, const Vector2& p) : CObject(t,p)
{
	platState = ps;
	waiting = w;
	justGotOffStop = false;
	float ydelta = 0.0f;

  if (waiting) {
    m_vVelocity.y = 0.0f;
  } else {
    if(platState == DOWN)
      m_vVelocity.y = -40.0f;
    else
      m_vVelocity.y = 40.0f;
  }
	
}

/// \turnaround
///
/// This is triggered when the elevator reaches an endpoint on the map
/// tells the elevator to go the opposite direction but also wait
/// for the player to tell it to move.
/// basically, turn around and get ready to go the other way.
/// also sets justGotOffStop to false so that when we next hit a stop point
/// it will stop.
void Elevator::turnAround()
{
  if (!waiting) {
    if (platState == UP) {
      platState = DOWN;
      waiting = true;
    } else {
      platState = UP;
      waiting = true;
    }
  }
	justGotOffStop = false;
}

/// \stop
///
/// if we arent stopped, then stop
void Elevator::stop()
{
  if (!stopped) {
    stopped = true;
  }
}

/// \stop
/// \param Vector2 pos -  position of the stop point that we set the elevators position to
/// set our velocity to 0
/// say elevator is waiting
/// say it is stopped
/// say that we just got off stop
/// set our ydelta to zero
void Elevator::stop(Vector2 pos)
{
	if (!waiting) {
		m_vPos = pos;
		m_vVelocity = Vector2(0.0f, 0.0f);
		waiting = true;
		stopped = true;
		justGotOffStop = true;
		ydelta = 0.0f;
	}
}


/// \goUp
/// 
///if we are not going down or not waiting or we are stopped
///then we should go up and then not wait
void Elevator::goUp()
{
	// 
	if (!(platState == DOWN && waiting) || stopped) {
    platState = UP;
    waiting = false;
  }

}

void Elevator::goDown()
{
  if (!(platState == UP && waiting) || stopped) {
    platState = DOWN;
    waiting = false;
  }
}

void Elevator::move(const float & t)
{
	//const float t = m_pTimer->frametime();
	//const float t = 1 / 60.0f;

	if (!waiting)
	{
		if (platState == UP)
			m_vVelocity = Vector2(0.0f, 80.0f);
		else
			m_vVelocity = Vector2(0.0f, -80.0f);

    elevatorVel = m_vVelocity.y;

		m_vPos += m_vVelocity * t;
		ydelta += abs(m_vVelocity.y * t);
		if (ydelta > 33.0f) //used to be 32 but i gave it some breathing room
		{
			justGotOffStop = false;
		}

		const Vector2 topleft(-m_vRadius.x, m_vRadius.y);
		const Vector2 bottomrt(m_vRadius.x, -m_vRadius.y);
		BoundingBox::CreateFromPoints(m_Aabb, topleft + m_vPos, bottomrt + m_vPos);

		m_Sphere.Center = (Vector3)m_vPos;
	}//if
  else {
    elevatorVel = 0.0f;
  }
}