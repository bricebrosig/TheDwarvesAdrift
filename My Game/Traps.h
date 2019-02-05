#pragma once
#include "Object.h"

class Trap : public CObject
{
	friend class CObjectManager;
private:
	bool state; //true is on  && false is off
	int index; //what switch it is associated with
public:
	Trap(eSpriteType, const Vector2&);
	Trap(bool, int, eSpriteType, const Vector2&);
  void move(const float &);//slightly different than the CObject move. just tells the trap to animate
  void animate();
	void flipState();
	bool getState();
	int getIndex();
	void setState(bool);
	void setIndex(int);
};

class Switch : public CObject
{
	friend class CObjectManager;
private:
	bool state;
	int index;
public:
	Switch(eSpriteType, const Vector2&);
	Switch(bool, int, eSpriteType, const Vector2&);
	bool getState();
	void setState(bool);
	int getIndex();
	void setIndex(int);
	void flipSwitch();
	void flipTrapSwitch();
};