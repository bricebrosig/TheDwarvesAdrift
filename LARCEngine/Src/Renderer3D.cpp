/// \file Renderer3D.cpp
/// \brief Code for the 3D renderer CRenderer3D.

#include "Renderer3D.h"
#include "TextureDesc.h"
#include "Helpers.h"

#include "ATGColors.h"
#include "ReadData.h"

#include <WICTextureLoader.h>

#include "Abort.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

#pragma warning(disable : 4238)

#pragma region Local namespace

namespace{
  struct Vertex{
    XMFLOAT4 position;
    XMFLOAT2 texcoord;
  };//Vertex

  vector<uint8_t> LoadBGRAImage(const wchar_t* filename, uint32_t& width, uint32_t& height){
    ComPtr<IWICImagingFactory> wicFactory;
    DX::ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory)));

    ComPtr<IWICBitmapDecoder> decoder;
    DX::ThrowIfFailed(wicFactory->CreateDecoderFromFilename(filename, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf()));

    ComPtr<IWICBitmapFrameDecode> frame;
    DX::ThrowIfFailed(decoder->GetFrame(0, frame.GetAddressOf()));

    DX::ThrowIfFailed(frame->GetSize(&width, &height));

    WICPixelFormatGUID pixelFormat;
    DX::ThrowIfFailed(frame->GetPixelFormat(&pixelFormat));

    uint32_t rowPitch = width * sizeof(uint32_t);
    uint32_t imageSize = rowPitch * height;

    std::vector<uint8_t> image;
    image.resize(size_t(imageSize));

    if(memcmp(&pixelFormat, &GUID_WICPixelFormat32bppBGRA, sizeof(GUID)) == 0)
      DX::ThrowIfFailed(frame->CopyPixels(0, rowPitch, imageSize, reinterpret_cast<BYTE*>(image.data())));
    else{
      ComPtr<IWICFormatConverter> formatConverter;
      DX::ThrowIfFailed(wicFactory->CreateFormatConverter(formatConverter.GetAddressOf()));

      BOOL canConvert = FALSE;
      DX::ThrowIfFailed(formatConverter->CanConvert(pixelFormat, GUID_WICPixelFormat32bppBGRA, &canConvert));
      if(!canConvert)
        throw std::exception("CanConvert");

      DX::ThrowIfFailed(formatConverter->Initialize(frame.Get(), GUID_WICPixelFormat32bppBGRA,
        WICBitmapDitherTypeErrorDiffusion, nullptr, 0, WICBitmapPaletteTypeMedianCut));

      DX::ThrowIfFailed(formatConverter->CopyPixels(0, rowPitch, imageSize, reinterpret_cast<BYTE*>(image.data())));
    } //else

    return image;
  } //vector
} //namespace

#pragma endregion

//////////////////////////////////////////////////////////////////////////////////////////////

CRenderer3D::CRenderer3D(){ 
  m_pDeviceResources = make_unique<DX::DeviceResources>();
  m_pDeviceResources->RegisterDeviceNotify(this);
} //constructor

CRenderer3D::~CRenderer3D(){
  delete m_pResourceUpload;

  if(m_pDeviceResources)
    m_pDeviceResources->WaitForGpu();
} //destructor

/// Do the necessary voodoo to set up Direct3D 12 rendering.

///\param bUseDepthStencil true to use z-buffering.

void CRenderer3D::Initialize(bool bUseDepthStencil){
  m_bUseDepthStencil = bUseDepthStencil;

  m_pDeviceResources->CreateDeviceResources();  	
  m_pD3DDevice = m_pDeviceResources->GetD3DDevice();
  m_pGraphicsMemory = make_unique<GraphicsMemory>(m_pD3DDevice);
  m_pStates = make_unique<CommonStates>(m_pD3DDevice);
  
  m_RenderTargetState = RenderTargetState(
    m_pDeviceResources->GetBackBufferFormat(), 
    bUseDepthStencil? m_pDeviceResources->GetDepthBufferFormat(): DXGI_FORMAT_UNKNOWN); 

  m_pDescriptorHeap = make_unique<DescriptorHeap>(m_pD3DDevice,
    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 
    D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
    1024); //TODO: This number was pulled out of the air.
  
  m_pResourceUpload = new ResourceUploadBatch(m_pD3DDevice);

  m_pDeviceResources->SetWindow(m_Hwnd, m_nWinWidth, m_nWinHeight);
  m_pDeviceResources->CreateWindowSizeDependentResources();

  CreateDeviceDependentResources();
} //Initialize

/// Notify the resource upload object that uploading is about to begin.
/// The uploads will take place in a separate thread.

void CRenderer3D::BeginResourceUpload(){
  m_pResourceUpload->Begin();
} //BeginResourceUpload

/// Notify the resource upload object that uploading is over, and
/// wait for any uploads in progress to finish.

void CRenderer3D::EndResourceUpload(){
  auto finished = m_pResourceUpload->End(m_pDeviceResources->GetCommandQueue());
  finished.wait(); //wait for the upload thread to terminate
} //EndResourceUpload

/// Given a COM pointer to a texture that has just been loaded using CreateDDSTexture
/// of CreateWICTexture, append it to the end of texture list, create a shader resource
/// view for it, and record the texture's index, resource descriptor index,
/// and dimensions in the texture descriptor.
/// \param p [in] Pointer to the D3D12 resource for the texture.
/// \param tDesc [out] Descriptor for the texture pointed to by p.

void CRenderer3D::ProcessTexture(ComPtr<ID3D12Resource> p, CTextureDesc& tDesc){
  m_pTexture.push_back(p);

  CreateShaderResourceView(m_pD3DDevice, p.Get(), 
    m_pDescriptorHeap->GetCpuHandle(m_nNumResourceDesc)); 
  
  tDesc.m_nTextureIndex = (int)m_pTexture.size() - 1;
  tDesc.m_nResourceDescIndex = m_nNumResourceDesc;
  
  const XMUINT2 size = GetTextureSize(p.Get());
  tDesc.m_nWidth = size.x;
  tDesc.m_nHeight = size.y;

  ++m_nNumResourceDesc;
} //ProcessTexture

/// Load a texture from a DirectDraw Surface file.
/// Aborts if the file is not found.
/// \param szFileName [in] Wide file name.
/// \param tDesc [out] Descriptor for the texture pointed to by p.

void CRenderer3D::CreateDDSTexture(_In_z_ const wchar_t* szFileName, CTextureDesc& tDesc){
  ComPtr<ID3D12Resource> pTexture;

  const HRESULT hr = CreateDDSTextureFromFile(m_pD3DDevice, *m_pResourceUpload, 
    szFileName, pTexture.ReleaseAndGetAddressOf());

  if(FAILED(hr))
    ABORTW(L"Couldn't open DDS texture file \"%s\".", szFileName);
  
  ProcessTexture(pTexture, tDesc);
} //CreateDDSTexture

/// Load a texture from a WIC file.
/// Aborts if the file is not found.
/// \param szFileName [in] Wide file name.
/// \param tDesc [out] Texture descriptor.

void CRenderer3D::CreateWICTexture(_In_z_ const wchar_t* szFileName, CTextureDesc& tDesc){
  ComPtr<ID3D12Resource> pTexture;

  const HRESULT hr = CreateWICTextureFromFile(m_pD3DDevice, *m_pResourceUpload, 
    szFileName, pTexture.ReleaseAndGetAddressOf(), true);

  if(FAILED(hr))
    ABORTW(L"Couldn't open WIC texture file \"%s\".", szFileName);

  ProcessTexture(pTexture, tDesc);
} //CreateWICTexture

/// Load a texture from a file.
/// Aborts if the file is not found.
/// \param filename [in] File name.
/// \param tDesc [out] Texture descriptor.

void CRenderer3D::LoadTextureFile(const char* filename, CTextureDesc& tDesc){
  wchar_t* wfilename = nullptr; //wide file name
  MakeWideFileName(filename, wfilename); //convert the former to the latter

  if(strcmp(strrchr(filename, '.'), ".dds") == 0) //check the file extension
    CreateDDSTexture(wfilename, tDesc); //DirectDraw Surface format
  else
    CreateWICTexture(wfilename, tDesc); //hopefully a WIC format

  delete [] wfilename; //clean up
} //LoadTextureFile

/// Load a texture from a file specified in gamesettings.xml.
/// Aborts if gamesettings.xml has not been loaded correctly.
/// \param name [in] Name field in a sprite tag.
/// \param tDesc [out] Texture descriptor.

void CRenderer3D::LoadTexture(const char* name, CTextureDesc& tDesc){
  if(m_pXmlSettings == nullptr)
    ABORT("Cannot access gamesettings.xml.");

  XMLElement* grouptag = m_pXmlSettings->FirstChildElement("textures"); 

  if(grouptag == nullptr)
    ABORT("Cannot find <textures> tag in gamesettings.xml.");

  XMLElement* texturetag = grouptag->FirstChildElement("texture");

  while(texturetag && strcmp(name, texturetag->Attribute("name")))
    texturetag = texturetag->NextSiblingElement("texture");

  if(texturetag == nullptr)  
    ABORT("Cannot find <texture name=\"%s\"> in gamesettings.xml.", name);

  LoadTextureFile(texturetag->Attribute("file"), tDesc);
} //LoadTexture

/// Load the font for the screen text from a font file specified in gamesettings.xml.

void CRenderer3D::LoadScreenFont(){
  XMLElement* tag = m_pXmlSettings->FirstChildElement("font"); 
  if(tag == nullptr)return; //no tag, so bail

  const char* filename = tag->Attribute("file");
  const size_t newsize = strlen(filename) + 1;  
  wchar_t* wfilename = new wchar_t[newsize]; //wide file name
  size_t n;
  mbstowcs_s(&n, wfilename, newsize, filename, _TRUNCATE); //convert file name to wide characters

  m_pFont = make_unique<SpriteFont>(m_pD3DDevice, *m_pResourceUpload,
    wfilename,
    m_pDescriptorHeap->GetCpuHandle(m_nNumResourceDesc),
    m_pDescriptorHeap->GetGpuHandle(m_nNumResourceDesc));

  ++m_nNumResourceDesc;

  delete [] wfilename;
} //LoadScreenFont

/// Create SpriteBatch, PrimitiveBatch and set their viewports.
/// Load the text font.

void CRenderer3D::CreateDeviceDependentResources(){
  m_pResourceUpload->Begin();

  RenderTargetState rtState(m_pDeviceResources->GetBackBufferFormat(), 
    m_pDeviceResources->GetDepthBufferFormat());

  SpriteBatchPipelineStateDescription spd(rtState, &CommonStates::NonPremultiplied);
  m_pSpriteBatch = make_unique<SpriteBatch>(m_pD3DDevice, *m_pResourceUpload, spd);
  
  SpriteBatchPipelineStateDescription tpd(rtState);
  m_pTextSpriteBatch = make_unique<SpriteBatch>(m_pD3DDevice, *m_pResourceUpload, tpd);

  m_pPrimitiveBatch = make_unique<PrimitiveBatch<VertexPositionColor>>(m_pD3DDevice);

  auto viewport = m_pDeviceResources->GetScreenViewport();

  m_pSpriteBatch->SetViewport(viewport);
  m_pTextSpriteBatch->SetViewport(viewport); 
  
  LoadScreenFont(); //font for screen text

  //upload the resources to the GPU

  auto finished = m_pResourceUpload->End(m_pDeviceResources->GetCommandQueue());
  finished.wait(); //wait for the upload thread to terminate
} //CreateDeviceDependentResources

/// \param text Null terminated text string.
/// \param p position of text.
/// \param color Text color, defaults to black.

void CRenderer3D::DrawScreenText(const char* text, const Vector2& p, XMVECTORF32 color){
  wchar_t* buffer = nullptr; //wide file name
  MakeWideFileName(text, buffer);
  DrawScreenText(buffer, p, color);
  delete [] buffer;
} //DrawScreenText

/// \param text Null terminated wide text string.
/// \param p position of text.
/// \param color Text color, defaults to black.

void CRenderer3D::DrawScreenText(const wchar_t* text, const Vector2& p, XMVECTORF32 color){
  if(m_pFont == nullptr)return; //bail out

  m_pFont->DrawString(m_pSpriteBatch.get(), text, p, color);
} //DrawScreenText

/// \param text Null terminated text string.
/// \param color Text color, defaults to black.

void CRenderer3D::DrawCenteredText(const char* text, XMVECTORF32 color){
  wchar_t* buffer = nullptr; //wide file name
  MakeWideFileName(text, buffer);
  DrawCenteredText(buffer, color);
  delete [] buffer;
} //DrawCenteredText

/// \param text Null terminated wide text string.
/// \param color Text color, defaults to black.

void CRenderer3D::DrawCenteredText(const wchar_t* text, XMVECTORF32 color){
  if(m_pFont == nullptr)return; //bail out

  const RECT r = m_pFont->MeasureDrawBounds(text, XMFLOAT2(0.f, 0.f)); //bounding rectabgle
  const float w = (float)r.right - r.left; //text width in pixels
  const float h = (float)r.bottom - r.top; //text height in pixels
  const XMFLOAT2 pos(m_vWinCenter.x - w/2, m_vWinCenter.y - h); //text position

  m_pFont->DrawString(m_pSpriteBatch.get(), text, pos, color);
} //DrawCenteredText

/// Must be called at the start of each animation frame
/// to initialize the render pipeline.

void CRenderer3D::BeginFrame(){  
  m_pDeviceResources->Prepare();

  m_pCommandList = m_pDeviceResources->GetCommandList();
  m_pHeaps[0] = m_pDescriptorHeap->Heap();
  m_pHeaps[1] = m_pStates->Heap();
  m_pCommandList->SetDescriptorHeaps(_countof(m_pHeaps), m_pHeaps);
  
  auto rtvDescriptor = m_pDeviceResources->GetRenderTargetView();
  auto dsvDescriptor = m_pDeviceResources->GetDepthStencilView();
  
  m_pCommandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, 
    m_bUseDepthStencil? &dsvDescriptor: nullptr);
  m_pCommandList->ClearRenderTargetView(rtvDescriptor, m_f32BgColor, 0, nullptr);
  m_pCommandList->ClearDepthStencilView(dsvDescriptor, 
    D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

  auto viewport = m_pDeviceResources->GetScreenViewport();
  auto scissorRect = m_pDeviceResources->GetScissorRect();

  m_pCommandList->RSSetViewports(1, &viewport);
  m_pCommandList->RSSetScissorRects(1, &scissorRect);
} //BeginFrame

/// Must be called at the end of each animation frame
/// to render and present the frame.

void CRenderer3D::EndFrame(){
  m_pDeviceResources->Present(); //show the new frame
  m_pGraphicsMemory->Commit(m_pDeviceResources->GetCommandQueue());
} //EndFrame

/// \param color New background color.

void CRenderer3D::SetBgColor(const XMVECTORF32& color){
  m_f32BgColor = color;
} //SetBgColor

/////////////////////////////////////////////////////////////////////////////////////////

/// TODO: fix this, it ain't complete

void CRenderer3D::OnDeviceLost(){
  m_pFont.reset();
  m_pSpriteBatch.reset();
  m_pPrimitiveBatch.reset();
  m_pTextSpriteBatch.reset();
  m_pDescriptorHeap.reset();
  m_pStates.reset();
  m_pGraphicsMemory.reset();

  for(auto texture: m_pTexture)
    texture.Reset();
} //OnDeviceLost

/// TODO: fix this, it ain't complete

void CRenderer3D::OnDeviceRestored(){
} //OnDeviceRestored
