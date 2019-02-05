/// \file Sprite.cpp
/// \brief Code for the sprite class CSprite.

#include "Sprite.h"
#include "Renderer3D.h"

#include "Abort.h"

/// Create the texture descriptor array.
/// \param n Number of animation frames.

CSprite::CSprite(size_t n):
  m_nNumFrames(n),
  m_pTextureDesc(new CTextureDesc[n]){
} //constructor

/// Clean up the texture descriptor array.

CSprite::~CSprite(){
  delete [] m_pTextureDesc;
} //destructor

/// Reference function for the texture descriptor for an animation frame.
/// Note that the texture descriptor can be modified using the
/// reference returned by this function.
/// \param i Frame number. Defaults to zero.
/// \return Reference to the texture descriptor for frame i.

CTextureDesc& CSprite::GetTextureDesc(size_t i){
  return m_pTextureDesc[i];
} //GetTextureDesc

/// Reader function for the number of animation frames.
/// \return Number of animation frames.

size_t CSprite::GetNumFrames(){
  return m_nNumFrames;
} //GetNumFrames

/// Reader function for the width and height of a frame.
/// \param n [in] Frame number, assumed to be in range.
/// \param x [out] Width of frame in pixels.
/// \param y [out] Height of frame in pixels.

void CSprite::GetSize(unsigned n, float& x, float& y){
  x = (float)m_pTextureDesc[n].m_nWidth;
  y = (float)m_pTextureDesc[n].m_nHeight;
} //GetSize

/// Reader function for the width of a frame.
/// \param n Frame number, assumed to be in range, defaults to zero.
/// \return Width of frame in pixels.

float CSprite::GetWidth(unsigned n){
  return (float)m_pTextureDesc[n].m_nWidth;
} //GetWidth

/// Reader function for the height of a frame.
/// \param n Frame number, assumed to be in range, defaults to zero.
/// \return Height of frame in pixels.

float CSprite::GetHeight(unsigned n){
  return (float)m_pTextureDesc[n].m_nHeight;
} //GetHeight