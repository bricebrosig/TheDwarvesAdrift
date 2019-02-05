/// \file Sprite.h
/// \brief Interface for the sprite class CSprite.

#pragma once

#include "Defines.h"

#include "TextureDesc.h"
#include "Component.h"

#include "DirectXCollision.h"

using namespace DirectX;

/// \brief The sprite. 
///
/// A sprite consists of textures for its animation frames
/// and the associated texture information.

class CSprite: public CComponent{
  private:
    CTextureDesc* m_pTextureDesc = nullptr; ///< Texture descriptor array; one descriptor per frame.
    size_t m_nNumFrames = 1; //< Number of animation frames.

  public:
    CSprite(size_t n); ///< Contructor.
    ~CSprite(); ///< Destructor.
    
    CTextureDesc& GetTextureDesc(size_t i=0); ///< Get texture descriptor.

    float GetWidth(unsigned n=0); ///< Get sprite width.
    float GetHeight(unsigned n=0); ///< Get sprite height.
    void GetSize(unsigned n, float& x, float& y); ///< Get sprite width and height.
    size_t GetNumFrames(); ///< Get number of frames.
}; //CSprite

