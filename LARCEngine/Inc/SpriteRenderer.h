/// \file SpriteRenderer.h
/// \brief Interface for the sprite renderer CSpriteRenderer.

#pragma once

#include "Sprite.h"
#include "Renderer3D.h"

///\brief The sprite renderer class.
///
/// A renderer that will draw sprites in 2D and 3D.

class CSpriteRenderer: public CRenderer3D{
  public:

    /// \brief Render mode type.
    ///
    /// Render modes supported are batched 2D (which uses SpriteBatch
    /// from DirectXTK 12 for speed), and unbatched 2D and 3D.

    enum eRenderMode{Batched2D, Unbatched2D, Unbatched3D}; 

  protected: 
    eRenderMode m_eRenderMode; ///< Sprite render mode.

    CBaseCamera* m_pCamera = nullptr; ///< Pointer to the camera.
    
    CSprite** m_pSprite = nullptr; ///< Sprite pointers.
    size_t m_nNumSprites = 0; ///< Number of sprites.

    unique_ptr<BasicEffect> m_pSpriteEffect; ///< Sprite effect.

    CSprite* Load(unsigned index, const char* file, const char* ext, int frames); ///< Load sprite.
    void CreateEffect(); ///< Create effect.

  protected:
    GraphicsResource m_VertexBuffer; ///< Vertex buffer.
    GraphicsResource m_IndexBuffer; ///< Index buffer.
    UINT m_nIndexCount; ///< Number of indexes in the index buffer.

    shared_ptr<D3D12_VERTEX_BUFFER_VIEW> m_pVBufView; ///< Vertex buffer view.
    shared_ptr<D3D12_INDEX_BUFFER_VIEW>  m_pIBufView; ///< Index buffer view.

    float m_fCurZ = FLT_MAX; ///< Current depth for unbatched 2D rendering.
    
    void CreateVertexBuffer(); ///< Create vertex buffer.
    void CreateIndexBuffer();  ///< Create index buffer.

  public:
    CSpriteRenderer(eRenderMode mode); ///< Constructor.
    virtual ~CSpriteRenderer(); ///< Destructor.
    
    void Initialize(size_t n); ///< Initialize.

    void BeginFrame(); ///< Begin frame.
    void EndFrame(); ///< End frame.
    
    void Draw(const CSpriteDesc2D& sd); ///< Draw single 2D sprite.
    void Draw(int n, const Vector2& pos, float a=0.0f); ///< Draw single 2D sprite.
    void DrawLine(unsigned n, const Vector2& p0, const Vector2& p1); ///< Draw 2D line.

    void Draw(const CSpriteDesc3D& sd); ///< Draw single 3D sprite.
    void Draw(vector<CSpriteDesc3D>& renderlist); ///< Draw list of 3D sprites.

    void SetCameraPos(const Vector3& pos); ///< Set camera position.
    const Vector3& GetCameraPos(); ///< Get camera position.

    void Load(unsigned n, const char* name); ///< Load sprite.
    
    float GetWidth(unsigned n); ///< Get sprite width.
    float GetHeight(unsigned n); ///< Get sprite height.
    void GetSize(unsigned n, float& x, float& y); ///< Get sprite size.
    void GetSize(unsigned n, unsigned m, float& x, float& y); ///< Get sprite size.

    size_t GetNumFrames(unsigned n);  ///< Get number of frames.

    BoundingBox GetAabb(int n, int m); ///< Get bounding box.
    bool BoxInFrustum(const BoundingBox& box); ///< Does the box overlap the view frustum?
}; //CSpriteRenderer
