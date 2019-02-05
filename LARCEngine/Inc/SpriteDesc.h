/// \file Sprite.h
/// \brief Interface for the sprite descriptor classes.

#pragma once

#include "D3DDefines.h"

namespace{

  /// \brief The base sprite descriptor. 
  ///
  /// Describes the sprite draw parameters shared
  /// by 2D and 3D sprites, even the obscure ones that aren't
  /// used very often (such as alpha and tint).

  class CSpriteDesc{
    public:
      unsigned m_nSpriteIndex = 0; ///< Sprite index.
      unsigned m_nCurrentFrame = 0; ///< Current animation frame.

      float m_fXScale = 1.0f; ///< Horizontal scale.
      float m_fYScale = 1.0f; ///< Vertical scale.

      float m_fRoll = 0.0f; ///< Z-axis rotation.

      XMFLOAT4 m_f4Tint = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); ///< Color tint.
      float m_fAlpha = 1.0f; ///< Alpha channel, 0.0f for transparent, 1.0f for opaque.
  }; //CSpriteDesc
}; //namespace

/// \brief The 2D sprite descriptor. 
///
/// Describes the sprite draw parameters for 2D sprites.

class CSpriteDesc2D: public CSpriteDesc{
  public:
    Vector2 m_vPos = Vector2::Zero; ///< Position.
}; //CSpriteDesc2D

/// \brief The 3D sprite descriptor. 
///
/// Describes the sprite draw parameters for 3D sprites.

class CSpriteDesc3D: public CSpriteDesc{
  public:
    Vector3 m_vPos = Vector3::Zero; ///< Position.
    BoundingBox m_sAabb; ///< Axially aligned bounding box.

    float m_fYaw = 0.0f; ///< Y-axis rotation.
    float m_fPitch = 0.0f; ///< X-axis rotation.
    
    CSpriteDesc3D(const CSpriteDesc2D& d, float z); ///< Constructor.
    CSpriteDesc3D(); ///< Default constructor.
}; //CSpriteDesc3D
