/// \file Renderer3D.h
/// \brief Interface for the 3D renderer CRenderer3D.

#pragma once

#include "D3DDefines.h"
#include "Settings.h"
#include "BaseCamera.h"
#include "Component.h"
#include "CameraCommon.h"
#include "Window.h"
#include "TextureDesc.h"
#include "SpriteDesc.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace DX;

/// \brief The renderer.
///
/// The renderer uses DirectX 12 and DirectXTK 12.

class CRenderer3D: 
  public IDeviceNotify, 
  public CComponent,
  public CCameraCommon,
  public CWindow
{
  private:
    vector<ComPtr<ID3D12Resource>> m_pTexture; ///< Texture list.
    bool m_bUseDepthStencil; ///< Whether to use z-buffering.

    void CreateDeviceDependentResources(); ///< Create device dependent resources.

  protected:
    unique_ptr<GraphicsMemory> m_pGraphicsMemory; ///< Pointer to graphics memory.
    unique_ptr<DescriptorHeap> m_pDescriptorHeap; ///< Pointer to a descriptor heap.
    int m_nNumResourceDesc = 0; ///< Number of resource descriptors in descriptor heap.

    unique_ptr<CommonStates> m_pStates; ///< Pointer to render state object.

    unique_ptr<SpriteBatch> m_pSpriteBatch; ///< SpriteBatch object for rendering sprites in screen space (from the DirectXTK).
    unique_ptr<PrimitiveBatch<VertexPositionColor>> m_pPrimitiveBatch; ///< PrimitiveBatch  object from the DirectXTK.
    unique_ptr<SpriteBatch> m_pTextSpriteBatch; ///< SpriteBatch object for rendering text in screen space.
    unique_ptr<SpriteFont> m_pFont; ///< Text font.

    unique_ptr<DeviceResources> m_pDeviceResources; ///< Pointer to device resources.
    ID3D12Device* m_pD3DDevice; ///< Pointer to the D3D device.
    ResourceUploadBatch* m_pResourceUpload; ///< Pointer to the resource upload batch object.

    ID3D12GraphicsCommandList* m_pCommandList = nullptr; ///< Pointer to the command list.
    ID3D12DescriptorHeap* m_pHeaps[2]; ///< A pair of pointers to heaps.
    
    RenderTargetState m_RenderTargetState; ///< The render target state.
    
    XMVECTORF32 m_f32BgColor = Colors::SkyBlue; ///< The default background color.

    void CreateDDSTexture(_In_z_ const wchar_t* szFileName, CTextureDesc& tDesc); ///< Load a texture from a DirectDraw surface file (contains mipmaps).
    void CreateWICTexture(_In_z_ const wchar_t* szFileName, CTextureDesc& tDesc); ///< Load a texture from a an image file (does not contain mipmaps).
    void ProcessTexture(_In_ ComPtr<ID3D12Resource> p, CTextureDesc& tDesc); ///< Process a loaded texture.
    
    void LoadScreenFont(); ///< Load screen font.

  public:
    CRenderer3D(); ///< Constructor.
    ~CRenderer3D(); ///< Destructor.
    
    void Initialize(bool bUseDepthStencil = true); ///< Initialize renderer.

    void LoadTexture(const char* name, CTextureDesc& tDesc); ///< Load texture.
    void LoadTextureFile(const char* filename, CTextureDesc& tDesc); ///< Load texture from file.
    
    void BeginResourceUpload(); ///< Begin uploading textures.
    void EndResourceUpload(); ///< End uploading textures.

    virtual void BeginFrame(); ///< Begin frame.
    virtual void EndFrame(); ///< End frame.

    void SetBgColor(const XMVECTORF32& color); ///< Set default background color.

    void DrawScreenText(const char* text, const Vector2& p, XMVECTORF32 color=Colors::Black); ///< Draw screen text.
    void DrawScreenText(const wchar_t* text, const Vector2& p, XMVECTORF32 color=Colors::Black); ///< Draw screen text, wide version.

    void DrawCenteredText(const char* text, XMVECTORF32 color=Colors::Black); ///< Draw centered text.
    void DrawCenteredText(const wchar_t* text, XMVECTORF32 color=Colors::Black); ///< Draw centered text, wide version.

    // IDeviceNotify
    virtual void OnDeviceLost() override; ///< Handler for device lost notification.
    virtual void OnDeviceRestored() override; ///< Handler for device restored notification.
}; //CRenderer3D
