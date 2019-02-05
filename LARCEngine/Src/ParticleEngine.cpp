/// \file ParticleEngine.cpp
/// \brief Code for the particle engine class CParticleEngine.

#include "Helpers.h"
#include "ParticleEngine.h"
#include "Particle.h"

#define T0 class PARTICLE, class PARTICLEDESC, class VECTOR ///< Abbreviation.
#define T1 PARTICLE, PARTICLEDESC, VECTOR ///< Abbreviation.

template<T0> CParticleEngine<T1>::CParticleEngine(){
} //constructor

template<T0> CParticleEngine<T1>::~CParticleEngine(){
  for(auto const& p: m_stdList)
    delete p;
} //destructor

/// Pre-emptively trigger scale out of all particles of a given type.
/// \param n Sprite type of particle to be cleared.

template<T0> void CParticleEngine<T1>::clear(int n){
  for(auto const& p: m_stdList) //for each particle
    if(p->m_nSpriteIndex == n){ //if this is the droid we are looking for
      p->m_fBirthTime = m_pTimer->time(); 
      p->m_fLifeSpan *= p->m_fScaleOutFrac; 
      p->m_fScaleInFrac = 0.0f;
      p->m_fScaleOutFrac = 1.0f;
    } //if
} //clear

/// Pre-emptively trigger a fade out of all particles.
/// \param t Time over which to fade out.

template<T0> void CParticleEngine<T1>::clear(float t){
  for(auto const& p: m_stdList){ //for each particle
    p->m_fBirthTime = m_pTimer->time(); 
    p->m_fLifeSpan = t; 
    p->m_fFadeInFrac = 0.0f;
    p->m_fFadeOutFrac = 1.0f;
  } //for
} //clear

/// Create a particle given its particle descriptor.
/// \param d Particle descriptor.

template<T0> void CParticleEngine<T1>::create(const PARTICLEDESC& d){
  PARTICLE* p = new PARTICLE(d, m_pTimer->time()); //create new particle
  p->rescale(0.0f); //scale it correctly 
  m_stdList.push_back(p);
} //create

/// Get the remaining fraction of a particle's lifespan.
/// \param p Pointer to a particle.
/// \return Particle's age as a float between 0.0f and 1.0f.

template<T0> float CParticleEngine<T1>::GetLifeFraction(const PARTICLE* p){
  const float age = m_pTimer->time() - p->m_fBirthTime;
  return clamp(0.0f, age/p->m_fLifeSpan, 1.0f);
} //GetLifeFraction

/// Move all particles, cull any that have exceeded their lifespan,
/// and recompute the scale and transparency of the ones remaining.

template<T0> void CParticleEngine<T1>::step(){
  move(); //move all particles
  cull(); //cull the dead ones
  rescale(); //rescale the remainder
  fade(); //fade out
} //step

/// Move all particles in the particle list.

template<T0> void CParticleEngine<T1>::move(){ 
  for(auto const& p: m_stdList)
    p->move(m_pTimer->frametime()); 
} //move

/// Remove from the particle list and delete those
/// particles that have reached or exceeded their lifespan.

template<T0> void CParticleEngine<T1>::cull(){ 
  for(auto i=m_stdList.begin(); i!=m_stdList.end();)
    if(GetLifeFraction(*i) < 1.0f) //not dead
      ++i; //next particle
    else{ //"He's dead, Dave." --- Holly, from Red Dwarf
      delete *i; //delete particle
      i = m_stdList.erase(i); //remove from particle  list
    } //else
} //cull

/// Rescale all of the particles in the particle list
/// according to how much of their lifespans they have
/// lived up to this point.

template<T0> void CParticleEngine<T1>::rescale(){ 
  for(auto const& p: m_stdList)
    p->rescale(GetLifeFraction(p)); 
} //rescale

/// Fade out all particles in the particle list
/// according to how much of their lifespans they have
/// lived up to this point.

template<T0> void CParticleEngine<T1>::fade(){ 
  for(auto const& p: m_stdList)
    p->fade(GetLifeFraction(p)); 
} //fade

/// Translate all particles simultaneously by the same amount.
/// \param delta Translation amount.

template<T0> void CParticleEngine<T1>::translate(const VECTOR& delta){ 
  for(auto const& p: m_stdList)
    p->translate(delta); 
} //translate

//explicit template instantiations

template class CParticleEngine<CParticle2D, CParticleDesc2D, Vector2>;
template class CParticleEngine<CParticle3D, CParticleDesc3D, Vector3>;

/////////////////////////////////////////////////////////////////////////////////////

/// Save a pointer to the 2D renderer so that particles
/// can be drawn on command.
/// \param p Pointer to the 2D renderer.

CParticleEngine2D::CParticleEngine2D(CSpriteRenderer* p):
  m_pRenderer(p){
} //constructor

/// Draw all particles using the 2D renderer supplied in the constructor.

void CParticleEngine2D::Draw(){
  for(auto const& p: m_stdList) //for each object
    m_pRenderer->Draw((CSpriteDesc2D)*p); //append to render list
} //Draw
    
/////////////////////////////////////////////////////////////////////////////////////

/// Append the sprite descriptors of all particles to
/// the end of a render list.
/// \param renderlist A vector of 3D sprite descriptors.

void CParticleEngine3D::GetRenderList(vector<CSpriteDesc3D>& renderlist){
  for(auto const& p: m_stdList) //for each object
    renderlist.push_back(*(CSpriteDesc3D*)p); //append to render list
} //GetRenderList
