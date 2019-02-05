#pragma once

#include "Object.h"
#include "Common.h"
#include "ObjectManager.h"
#include "ComponentIncludes.h"

enum platformState {UP,DOWN};


/// \brief Elevator Class
///
///Class that handles the elevators in the game. 
///Requrie A LOT of state about what is happening to the elevator
///Chekcs when it is waiting for players to tell it move and whether it needs to stop at endspoints
///or stops points.
class Elevator : public CObject
{
	friend class CObjectManager; // touch my privates

private:
	platformState platState; ///elevator going up or down
	
  
public:
  bool stopped = false; ///is the elevator at a stop point
  bool waiting; /// is the elevator waiting for a player
	bool justGotOffStop; /// was the elevator previously on the stoppoint
	float ydelta; ///how much elevator has moved since being on stop point
	Elevator(platformState, bool, eSpriteType t, const Vector2& p);
	void turnAround(); ///if we are going up go down and visa versa
	void move(const float &); /// overwritten version of the objects move
	void goUp(); /// tell elevator stop waiting and move up
	void goDown(); /// tell elevator to stop waiting and move down
  void stop(); ///set state to stop
	void stop(Vector2); ///stop and go to this point
};