/// \file Particle.h
/// \brief Interface for the particle class CParticle.

#pragma once

#include "Defines.h"
#include "SpriteDesc.h"

/// \brief The particle descriptor.
///
/// The particle descriptor describes a particle. It's templated so
/// we can define a 2D and a 3D version with a minimum of fuss.

template<class VECTOR, class SPRITEDESC>
class CParticleDesc: public SPRITEDESC{
  public:
    VECTOR m_vVel = VECTOR::Zero; ///< Velocity.
    VECTOR m_vAccel = VECTOR::Zero; ///< Acceleration.

    float m_fFriction = 0.0f; ///< Coefficient of friction.
    float m_fRSpeed = 0.0f; ///< Rotational speed.
    float m_fLifeSpan = FLT_MAX; ///< Time it will live for.

    float m_fMaxScale = 1.0f; ///< Maximum scale.
    float m_fScaleInFrac = 0.0f; ///< Fraction of life growing in.
    float m_fScaleOutFrac = 0.0f; ///< Fraction of life shrinking out.

    float m_fFadeInFrac = 0.0f; ///< Fraction of life fading in.
    float m_fFadeOutFrac = 0.0f; ///< Fraction of life fading out.
}; //CParticleDesc

/// \brief A particle.
///
/// A particle is a sprite that exists for a brief time.
/// It can grow in and shrink out, and also fade in and
/// fade out using an alpha channel. It's templated so
/// we can define a 2D and a 3D version with a minimum of fuss.

template<class VECTOR, class SPRITEDESC>
class CParticle: public CParticleDesc<VECTOR, SPRITEDESC>{ 
  public:
    float m_fBirthTime; ///< Time of creation.

    CParticle(const CParticleDesc<VECTOR, SPRITEDESC>& d, float t); ///< Constructor.

    void move(float t); ///< Move and rotate.
    void rescale(float f); ///< Rescale.
    void fade(float f); ///< Fade.
    void translate(const VECTOR& delta); ///< Translate.
}; //CParticle

////////////////////////////////////////////////////////////////////

using CParticleDesc2D = CParticleDesc<Vector2, CSpriteDesc2D>; ///< 2D particle descriptor.
using CParticleDesc3D = CParticleDesc<Vector3, CSpriteDesc3D>; ///< 3D particle descriptor.

using CParticle2D = CParticle<Vector2, CSpriteDesc2D>; ///< 2D particle.
using CParticle3D = CParticle<Vector3, CSpriteDesc3D>; ///< 3D particle.
