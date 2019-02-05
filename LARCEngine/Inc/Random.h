/// \file Random.h
/// \brief Interface for the pseudorandom number generator CRandom.

#pragma once

#include "Component.h"
#include "Defines.h"

/// \brief A Pseudorandom Number Generator (PRNG for short).
///
/// CRandom is a simple pseudorandom number generator based on
/// xorshift128. It can be seeded with the time or, if reproducability
/// is desired (eg. when debugging), with a fixed seed.
/// It has functions for generating pseudorandom unsigned integers,
/// floats, unit-length vectors, and colors.

class CRandom: public CComponent{
  private: 
    unsigned m_uState[4]; ///< Current state.

  public:
    CRandom(); ///< Constructor.

    void srand(int seed=-1); ///< Seed the random number generator.

    unsigned randn(); ///< Get a random unsigned integer.
    unsigned randn(unsigned i, unsigned j); ///< Get random integer in \f$[i,j]\f$.
    float randf(); ///< Get a random floating point number.
    Vector2 randv(); ///< Get a random direction.
    XMFLOAT4 randclr(); ///< Get a random color.

    BYTE* rand128(); ///< Get 128 random bits.
}; //CRandom
