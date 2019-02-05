/// \file ParticleEngine.h
/// \brief Interface for the particle engine class CParticleEngine.

#pragma once

#include "Timer.h"
#include "Random.h"
#include "Component.h"
#include "Particle.h"
#include "SpriteRenderer.h"

#include <list>
#include <vector>

using namespace std;

/// \brief An abstract particle engine.
///
/// A particle engine is responsible for managing 
/// a system of particles. Its step function must
/// be called once per animation frame. Templates
/// are used to make this class independent of
/// operating system and renderer implementation.
/// \tparam PARTICLE Partice class.
/// \tparam PARTICLEDESC Partice descriptor.
/// \tparam VECTOR Vector.

template<class PARTICLE, class PARTICLEDESC, class VECTOR>
class CParticleEngine: public CComponent{
  protected:
    list<PARTICLE*> m_stdList; ///< Particle list.
    
    void move(); ///< Move all particles.
    float GetLifeFraction(const PARTICLE* p); ///< Get life fraction as a float between 0.0f and 1.0f.
    void cull(); ///< Cull old dead particles
    void rescale(); //< Compute new particle scales.
    void fade(); //< Compute new particle alpha channel.

  public:
    CParticleEngine(); ///< Constructor.
    virtual ~CParticleEngine(); ///< Destructor.
    
    void create(const PARTICLEDESC& d); //< Create particle.
    void step(); ///< Animation step.

    void clear(float t=0.1f); ///< Fade out all particles.
    void clear(int n); ///< Clear out a particular kind of particle. 
    
    void translate(const VECTOR& delta);
}; //CParticleEngine

/////////////////////////////////////////////////////////////////////////////////////

/// \brief The 2D particle engine.
///
/// The 2D particle engine needs a pointer to a 2D renderer
/// and will use this to draw particles.

class CParticleEngine2D:
  public CParticleEngine<CParticle2D, CParticleDesc2D, Vector2>
{
  private:
    CSpriteRenderer* m_pRenderer; ///< Pointer to a 2D renderer.

  public:
    CParticleEngine2D(CSpriteRenderer* p); ///< Constructor.
    void Draw(); ///< Draw particles.
}; //CParticleEngine2D

/////////////////////////////////////////////////////////////////////////////////////

/// \brief The 3D particle engine.
///
/// The 3D particle engine can deliver a vector of 3D sprite
/// descriptors that need to be rendered.

class CParticleEngine3D:
  public CParticleEngine<CParticle3D, CParticleDesc3D, Vector3>
{
  public:
    void GetRenderList(vector<CSpriteDesc3D>& renderlist); ///< Get render list.
}; //CParticleEngine3D