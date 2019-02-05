#include "GunTurret.h"
#include "ComponentIncludes.h"

GunTurret::GunTurret(facingDirection f, eSpriteType t, const Vector2& p) : CObject(t, p)
{
  m_nFacing = f;
  attackRange = 400.0f;
}

void GunTurret::move() {

}