/// \file Particle.cpp
/// \brief Code for the particle class CParticle.

#include "Particle.h"

#define T0 class VECTOR, class SPRITEDESC ///< Abbreviation.
#define T1 VECTOR, SPRITEDESC ///< Abbreviation.

/// Create a particle given a particle descriptor and
/// a creation time.
/// \param d Particle descriptor.
/// \param t Birth time.

template<T0> CParticle<T1>::CParticle(const CParticleDesc<VECTOR, SPRITEDESC>& d, float t):
  CParticleDesc<VECTOR, SPRITEDESC>(),
  m_fBirthTime(t)
{
  *((CParticleDesc<VECTOR, SPRITEDESC>*)(this)) = d;
} //constructor

/// Translate, apply acceleration, apply friction, and rotate.
/// \param t Time step.

template<T0> void CParticle<T1>::move(float t){
  SPRITEDESC::m_vPos += t*CParticle<T1>::m_vVel; //translate
  CParticle<T1>::m_vVel += t*100.0f*CParticle<T1>::m_vAccel; //accelerate
  CParticle<T1>::m_vVel *= (1.0f - t*CParticle<T1>::m_fFriction); //friction
  CParticle<T1>::m_fRoll += t*XM_2PI*CParticle<T1>::m_fRSpeed; //rotate
} //move

/// Rescale the particle. It grows to m_fMaxScale for the
/// first m_fScaleInFrac fraction of its life, and shrink to
/// zero for the last m_fScaleOutFrac of its life.
/// \param f Fraction of lifespan lived, from 0.0f to 1.0f inclusive.

template<T0> void CParticle<T1>::rescale(float f){
  float fScale = CParticle<T1>::m_fMaxScale; //initial scale

  if(f < CParticle<T1>::m_fScaleInFrac) //growing in
    fScale *= f/CParticle<T1>::m_fScaleInFrac;

  if(f > 1.0f - CParticle<T1>::m_fScaleOutFrac) //shrinking out
    fScale *= (1.0f - f)/CParticle<T1>::m_fScaleOutFrac;

  CParticle<T1>::m_fXScale = CParticle<T1>::m_fYScale = fScale; //scale both directions
} //rescale

/// Make the particle's alpha channel fade in or fade out over time.
/// \param f Fraction of lifespan lived, from 0.0f to 1.0f inclusive.

template<T0> void CParticle<T1>::fade(float f){
  if(f < CParticle<T1>::m_fFadeInFrac) //fading in
    CParticle<T1>::m_fAlpha = f/CParticle<T1>::m_fFadeInFrac;

  if(f > 1.0f - CParticle<T1>::m_fFadeOutFrac) //fading out
    CParticle<T1>::m_fAlpha = (1.0f - f)/CParticle<T1>::m_fFadeOutFrac;
} //fade

/// Move particle by a given distance and direction.
/// \param delta Vector amount to add to current position.

template<T0> void CParticle<T1>::translate(const VECTOR& delta){
  SPRITEDESC::m_vPos += delta;
} //translate

//explicit template instantiations

template class CParticleDesc<Vector2, CSpriteDesc2D>;
template class CParticleDesc<Vector3, CSpriteDesc3D>;

template class CParticle<Vector2, CSpriteDesc2D>;
template class CParticle<Vector3, CSpriteDesc3D>;
