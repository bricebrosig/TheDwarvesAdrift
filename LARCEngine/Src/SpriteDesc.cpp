/// \file Sprite.cpp
/// \brief Code for the sprite descriptor classes.

#include "SpriteDesc.h"

/// Default constructor.

CSpriteDesc3D::CSpriteDesc3D(){
} //constructor

/// Construct a 3D sprite descriptor from a 2D sprite descriptor and a depth.
/// \param d A 2D sprite descriptor.
/// \param z Depth.

CSpriteDesc3D::CSpriteDesc3D(const CSpriteDesc2D& d, float z){
  *(CSpriteDesc*)this = *(CSpriteDesc*)&d;
  m_vPos = Vector3(d.m_vPos.x, d.m_vPos.y, z);
  m_fRoll = d.m_fRoll;
} //constructor

