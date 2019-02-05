#pragma once

#include "Object.h"

/// \ brief class for the stationary gun turrets
/// they store how far away enemies need to be to begin firing
/// they then fire when they see an enemy in range.
/// they dont aim either. only shoot forward
class GunTurret :
  public CObject
{
  friend class CObjectManager;
private:
  float attackRange;

public:
  GunTurret(facingDirection f, eSpriteType t, const Vector2& p);
  void move();
  float getRange() { return attackRange; };
};

