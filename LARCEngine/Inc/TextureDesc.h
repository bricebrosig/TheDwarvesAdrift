/// \file TextureDesc.h
/// \brief Interface for the texture descriptor CTextureDesc.

#pragma once

/// \brief The texture descriptor.
///
/// The texture descriptor describes a texture,
/// most importantly (in DirectX 12) its texture index and
/// resource descriptor index.

class CTextureDesc{
  public:
    unsigned m_nTextureIndex = 0; ///< Texture index.
    unsigned m_nResourceDescIndex = 0; ///< Resource descriptor index.

    unsigned m_nWidth = 0; ///< Width in pixels.
    unsigned m_nHeight = 0; ///< Height in pixels.
}; //CTextureDesc
