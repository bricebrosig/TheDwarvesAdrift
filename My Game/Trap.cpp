#include "Traps.h"
#include "ComponentIncludes.h"
#include "ObjectManager.h"
#include "TileManager.h"

Trap::Trap(eSpriteType t, const Vector2& p) : CObject(t, p)
{
	state = false;
	index = -1;
}

Trap::Trap(bool st, int in, eSpriteType t, const Vector2& p) : CObject(t, p)
{
	state = st;
	index = in;
}

bool Trap::getState() {
	return state;
}

int Trap::getIndex() {
	return index;
}

void Trap::setState(bool st) {
	state = st;
}

void Trap::flipState() {
  kill();
}

void Trap::setIndex(int in) {
	index = in;
}

///////////////

Switch::Switch(eSpriteType t, const Vector2& p) : CObject(t, p)
{
	state = false;
	index = -1;
}

Switch::Switch(bool st, int in, eSpriteType t, const Vector2& p) : CObject(t, p)
{
	state = st;
	index = in;
}

bool Switch::getState() {
	return state;
}

void Switch::setState(bool st) {
	state = st;
}

int Switch::getIndex() {
	return index;
}

void Switch::setIndex(int in) {
	index = in;
}

/*
	flip the switch to opposite state
*/
void Switch::flipSwitch() {
	(state ? state = false : state = true);
}

void Switch::flipTrapSwitch() 
{
	if (state)
		state = false;
	else
		state = true;

	for (auto t : m_pObjectManager->getVecTraps())
	{
		if (((Trap*)t)->getIndex() == ((Switch*)this)->getIndex())
		{
			((Trap*)t)->flipState();
		}
	}
}

void Trap::animate() {

  // get the total number of frames for this sprite
  const size_t numFrames = m_pRenderer->GetNumFrames(m_nSpriteIndex);
  const float dt = 1000 * m_fFrameInterval / (1500 + fabs(m_vVelocity.x));

  // selects a random frame 
  if (m_pTimer->elapsed(m_fFrameTimer, dt))
    m_nCurrentFrame = (m_pRandom->randn()) % numFrames;
}

void Trap::move(const float &t)
{
  animate();
}