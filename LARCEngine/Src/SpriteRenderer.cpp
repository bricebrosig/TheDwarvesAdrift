/// \file SpriteRenderer.cpp
/// \brief Code for the sprite renderer CSpriteRenderer.

#include "SpriteRenderer.h"
#include "Abort.h"

/// Construct a 3D renderer and a base camera.
/// \param mode Sprite render mode.

CSpriteRenderer::CSpriteRenderer(eRenderMode mode): 
  CRenderer3D(),
  m_pCamera(new CBaseCamera),
  m_eRenderMode(mode)
{
  const float w = (float)m_nWinWidth;
  const float h = (float)m_nWinHeight;
  const float a = w/h; //aspect ratio

  if(mode == Unbatched3D)
    m_pCamera->SetPerspective(a, XM_PI/4.0f, 0.1f, 100000.0f);
  else m_pCamera->SetOrthographic(w, h, 0.1f, 20000.0f); 

  m_pCamera->MoveTo(Vector3(m_vWinCenter.x, m_vWinCenter.y, 0.0f));
} //constructor

/// Clean up resources from GPU and wait for completion.

CSpriteRenderer::~CSpriteRenderer(){
  for(int i=0; i<m_nNumSprites; i++)
    delete m_pSprite[i];

  delete [] m_pSprite;
  delete m_pCamera;

  m_pDeviceResources->WaitForGpu();
} //destructor

/// Reserve space for the sprites, create sprite effect
/// and create vertex and index buffers if the renderer is
/// not in batched mode.
/// \param n Number of sprites.

void CSpriteRenderer::Initialize(size_t n){
  CRenderer3D::Initialize();
  
  m_nNumSprites = n;
  m_pSprite = new CSprite*[n];
  
  for(int i=0; i<n; i++)
    m_pSprite[i] = nullptr;

  CreateEffect(); //create sprite effect
  
  if(m_eRenderMode != Batched2D){
    CreateVertexBuffer(); //create and load vertex buffer
    CreateIndexBuffer(); //create and load index buffer
  } //if
} //Initialize

/// Create a vertex buffer containing the four vertices of
/// a quad centered at the origin with unit width and height.
/// The vertices have position and texture coordinates.
/// Create the corresponding vertex buffer view.

void CSpriteRenderer::CreateVertexBuffer(){ 
  VertexPositionTexture vertex[4];
  Vector3 pos;
  Vector3 norm(-Vector3::UnitZ);
  Vector2 uv;

  pos = Vector3(0.5f, 0.5f, 0.0f);
  uv = Vector2(1.0f, 0.0f);
  vertex[0] = VertexPositionTexture(pos, uv);
  
  pos = Vector3(0.5f, -0.5f, 0.0f);
  uv = Vector2(1.0f, 1.0f);
  vertex[1] = VertexPositionTexture(pos, uv);
  
  pos = Vector3(-0.5f, 0.5f, 0.0f);
  uv = Vector2(0.0f, 0.0f);
  vertex[2] = VertexPositionTexture(pos, uv);
  
  pos = Vector3(-0.5f, -0.5f, 0.0f);
  uv = Vector2(0.0f, 1.0f);
  vertex[3] = VertexPositionTexture(pos, uv);

  auto verts = reinterpret_cast<const uint8_t*>(vertex);
  const size_t vbufsize = 4*sizeof(VertexPositionTexture);

  m_VertexBuffer = GraphicsMemory::Get().Allocate(vbufsize);
  memcpy(m_VertexBuffer.Memory(), verts, vbufsize);
  
  //create vertex buffer view
  m_pVBufView = make_shared<D3D12_VERTEX_BUFFER_VIEW>();
  m_pVBufView->BufferLocation = m_VertexBuffer.GpuAddress();
  m_pVBufView->StrideInBytes = static_cast<UINT>(sizeof(VertexPositionTexture));
  m_pVBufView->SizeInBytes = static_cast<UINT>(m_VertexBuffer.Size());
} //CreateVertexBuffer

/// Create an index buffer and index buffer view for the sprite quad.

void CSpriteRenderer::CreateIndexBuffer(){   
  unsigned short index[4] = {0, 1, 2, 3};
  
  auto indices = reinterpret_cast<const uint8_t*>(index);
  const size_t ibufsize = 4*sizeof(unsigned short);

  m_IndexBuffer = GraphicsMemory::Get().Allocate(ibufsize);
  memcpy(m_IndexBuffer.Memory(), indices, ibufsize);
  
  //create index buffer view
  m_pIBufView = make_shared<D3D12_INDEX_BUFFER_VIEW>();
  m_pIBufView->BufferLocation = m_IndexBuffer.GpuAddress();
  m_pIBufView->SizeInBytes = static_cast<UINT>(m_IndexBuffer.Size());
  m_pIBufView->Format = DXGI_FORMAT_R16_UINT;
} //CreateIndexBuffer

/// Construct a pipeline description for the sprite and use that to
/// create an effect. Set its projection matrix to be the same as
/// the global projection matrix.

void CSpriteRenderer::CreateEffect(){
  EffectPipelineStateDescription pd(
    &VertexPositionTexture::InputLayout,
    CommonStates::NonPremultiplied,
    CommonStates::DepthDefault,
    CommonStates::CullNone, 
    m_RenderTargetState);

  m_pSpriteEffect = make_unique<BasicEffect>(m_pD3DDevice, EffectFlags::Texture, pd);
  m_pSpriteEffect->SetProjection(XMLoadFloat4x4(&m_projection));
} //CreateEffect

/// Initialize the render pipeline and the SpriteBatch.

void CSpriteRenderer::BeginFrame(){  
  CRenderer3D::BeginFrame(); 
  m_pSpriteBatch->Begin(m_pCommandList);
  m_fCurZ = 10000.0f;  //initial depth for unbatched sprite rendering
} //BeginFrame

/// End the SpriteBatch frame and present.

void CSpriteRenderer::EndFrame(){ 
  m_pSpriteBatch->End();
  CRenderer3D::EndFrame();
} //EndFrame

/// Draw a sprite in 2D.
/// \param sd 2D sprite descriptor.

void CSpriteRenderer::Draw(const CSpriteDesc2D& sd){ 
  if(m_eRenderMode == Batched2D){
    const unsigned t = sd.m_nSpriteIndex;
  
    float w, h;
    GetSize(t, w, h);
    const XMUINT2 size((unsigned)w, (unsigned)h);
    const XMFLOAT2 origin(size.x/2.0f, size.y/2.0f);

    const unsigned index = m_pSprite[t]->
      GetTextureDesc(sd.m_nCurrentFrame).m_nResourceDescIndex;

    Vector4 tint = sd.m_f4Tint;
    tint.w = sd.m_fAlpha;

    Vector2 pos = sd.m_vPos - (Vector2)m_pCamera->GetPos(); //position relative to camera
    pos.y = m_nWinHeight/2 - pos.y; //convert to screen space for SpriteBatch
    pos.x += m_nWinWidth/2;

    m_pSpriteBatch->Draw(
      m_pDescriptorHeap->GetGpuHandle(index),
      size, pos, nullptr, tint, -sd.m_fRoll, origin, sd.m_fXScale);
  } //if

  else if(m_eRenderMode == Unbatched2D){
    Draw(CSpriteDesc3D(sd, m_fCurZ));
    m_fCurZ -= 0.1f; //enforce back-to-front drawing (Painter's algorithm)
  } //else if

  else ABORT("Don't call the 2D Draw function in sprite render mode Unbatched3D");
} //Draw

/// Shorthand for drawing a 2D sprite with only index, position,
/// and orientation. The other sprite descriptor fields are set 
/// to default values.
/// \param n Sprite index.
/// \param pos Position in world space.
/// \param a Orientation angle (roll) in radians.

void CSpriteRenderer::Draw(int n, const Vector2& pos, float a){
  CSpriteDesc2D desc;

  desc.m_nSpriteIndex = n;
  desc.m_vPos = pos;
  desc.m_fRoll = a;

  Draw(desc);
} //Draw

/// Draw a line by rendering multiple copies of a line sprite in
/// batched mode or by stretching it in unbatched mode
/// (because SpriteBatch won't scale in one dimension, only two).
/// The line sprite is assumed to be horizontal and fairly small.
/// \param n Line sprite index.
/// \param p0 Position of one end of the sprite in world space.
/// \param p1 Position of the other end of the sprite in world space.
  
void CSpriteRenderer::DrawLine(unsigned n, const Vector2& p0, const Vector2& p1){ 
  CSpriteDesc2D d; //sprite descriptor for line sprite
  d.m_nSpriteIndex = n; //line sprite index
 
  const Vector2 dv = p0 - p1; //vector from p1 to p0
  d.m_fRoll = atan2(dv.y, dv.x); //orientation angle

  const float w = GetWidth(n); //width of line sprite

  if(m_eRenderMode == Batched2D){
    const int m = (unsigned)ceil(dv.Length()/w); //number of copies to render
    const Vector2 delta = dv/(float)m; //offset between copies
    Vector2 u = delta; //direction of line
    u.Normalize(); //make it a unit vector

    d.m_vPos = p1 + 0.5f*w*u; //offset the first copy by half a width

    for(int i=0; i<m; i++){ //draw copies
      Draw(d); //draw copy
      d.m_vPos += delta; //advance along line
    } //for
  } //if
  
  else if(m_eRenderMode == Unbatched2D){
    d.m_fXScale = dv.Length()/w;
    d.m_vPos = 0.5f*(p0 + p1);
    Draw(d); //draw stretched line
  } //else if

  else ABORT("Don't call the 2D DrawLine function in sprite render mode Unbatched3D");
} //DrawLine

/// Draw a sprite in 3D.
/// \param sd 3D sprite descriptor.

void CSpriteRenderer::Draw(const CSpriteDesc3D& sd){
  CTextureDesc& td = m_pSprite[sd.m_nSpriteIndex]->
    GetTextureDesc(sd.m_nCurrentFrame);

  const float xscale = sd.m_fXScale*td.m_nWidth;
  const float yscale = sd.m_fYScale*td.m_nHeight;

  const XMVECTORF32 scale = {xscale, yscale, 1.0f}; 

  const XMVECTORF32 translate = {sd.m_vPos.x, sd.m_vPos.y, sd.m_vPos.z};

  const Quaternion q = Quaternion::CreateFromYawPitchRoll(
    sd.m_fYaw, sd.m_fPitch, sd.m_fRoll);

  const XMMATRIX world = XMMatrixTransformation(
    g_XMZero, Quaternion::Identity, scale, g_XMZero, q, translate);
  
  m_pSpriteEffect->SetTexture(
    m_pDescriptorHeap->GetGpuHandle(td.m_nResourceDescIndex), 
    m_pStates->AnisotropicClamp());

  m_pSpriteEffect->SetAlpha(sd.m_fAlpha);

  m_pSpriteEffect->SetWorld(world);
  m_pSpriteEffect->SetView(XMLoadFloat4x4(&m_view));

  m_pSpriteEffect->Apply(m_pCommandList);

  m_pCommandList->IASetVertexBuffers(0, 1, m_pVBufView.get());
  m_pCommandList->IASetIndexBuffer(m_pIBufView.get());
  m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  m_pCommandList->DrawIndexedInstanced(4, 1, 0, 0, 0);
} //Draw

/// \brief Comparison for depth sorting sprites.
///
/// Compare the sprites' Z coordinates.
/// \param p0 Pointer to sprite descriptor 0.
/// \param p1 Pointer to sprite descriptor 1.
/// \return true If sprite 0 is behind sprite 1.

bool IsBehind(const CSpriteDesc3D* p0, const CSpriteDesc3D* p1){
  return p0->m_vPos.z > p1->m_vPos.z;
} //IsBehind

/// Depth sort a render list using a vector of pointers, then
/// draw them from back to front.
/// \param renderlist A vector of 3D sprite descriptors of the sprites to be rendered.

void CSpriteRenderer::Draw(vector<CSpriteDesc3D>& renderlist){
  vector<CSpriteDesc3D*> sortedlist; //use pointers for faster sorting

  for(int i=0; i<renderlist.size(); i++) //duplicate render list but with pointers
    sortedlist.push_back(&(renderlist[i]));

  stable_sort(sortedlist.begin(), sortedlist.end(), IsBehind); //depth sort
  
  for(auto p: sortedlist) //from back to front
    Draw(*p); //draw them
} //Draw

/// \param pos New camera position.

void CSpriteRenderer::SetCameraPos(const Vector3& pos){
  m_pCamera->MoveTo(pos);
} //SetCameraPos

/// Reader function for camera position.
/// \return Camera position.

const Vector3& CSpriteRenderer::GetCameraPos(){
  return m_pCamera->GetPos();
} //GetCameraPos

/// Given a file name and extension such as "foo" and "bmp", read
/// in sprite frames from "foo1.bmp", "foo2.bmp," etc.
/// \param index Sprite index.
/// \param file File name prefix
/// \param ext File name extension
/// \param frames Number of frames

CSprite* CSpriteRenderer::Load(unsigned index, const char* file, const char* ext, int frames){
  m_pSprite[index] = new CSprite(frames); //get space in array for new sprite

  if(ext == nullptr)
    LoadTextureFile(file, m_pSprite[index]->GetTextureDesc());
  else{
    string s; //file name

    for(int i=0; i<frames; i++){ //for each frame
      s = file + to_string(i) + "." + ext;
      LoadTextureFile(s.c_str(), m_pSprite[index]->GetTextureDesc(i));
    } //for
  } //else

  return m_pSprite[index]; 
} //Load

/// Load information about the sprite from global variable g_xmlSettings, then
/// load the sprite images as per that information. Abort if something goes wrong.
/// \param index Sprite index.
/// \param name Object name in XML file.

void CSpriteRenderer::Load(unsigned index, const char* name){
  if(m_pXmlSettings == nullptr)
    ABORT("Cannot access gamesettings.xml.");

  XMLElement* pSpritesTag = m_pXmlSettings->FirstChildElement("sprites"); //sprites tag

  if(pSpritesTag == nullptr)
    ABORT("Cannot find <sprites> tag in gamesettings.xml");

  string path(pSpritesTag->Attribute("path")); //get path

  XMLElement* pSpriteTag = pSpritesTag->FirstChildElement("sprite");

  while(pSpriteTag != nullptr && strcmp(name, pSpriteTag->Attribute("name")))
    pSpriteTag = pSpriteTag->NextSiblingElement("sprite");

  CSprite* pSprite = nullptr;

  if(pSpriteTag != nullptr){ //got <sprite> tag with right name
    const string file = path + "\\" + pSpriteTag->Attribute("file");
    const char* extension = pSpriteTag->Attribute("ext");
    const int frames = max(1, pSpriteTag->IntAttribute("frames"));

    pSprite = Load(index, file.c_str(), extension, frames);
  } //if

  if(pSprite == nullptr)
    ABORT("Cannot load sprite \"%s\".\n", name);
} //Load

/// Reader function for number of frames in sprite.
/// \param n Sprite index.
/// \return Number of frames in sprite.

size_t CSpriteRenderer::GetNumFrames(unsigned n){
  return m_pSprite[n]? m_pSprite[n]->GetNumFrames(): 0;
} //GetNumFrames

/// Construct the AABB for a sprite frame.
/// \param n Sprite index.
/// \param m Frame number.
/// \return AABB for sprite frame.

BoundingBox CSpriteRenderer::GetAabb(int n, int m){
  BoundingBox aabb;
  aabb.Center = Vector3::Zero;

  Vector3 v = Vector3(0.0f, 0.0f, 0.001f);
  GetSize(n, m, v.x, v.y);
  aabb.Extents = v;

  return aabb;
} //GetAabb

/// Reader function for the width and height of frame zero of a sprite.
/// \param n [in] Sprite index, assumed to be in range.
/// \param x [out] Width of frame in pixels.
/// \param y [out] Height of frame in pixels.

void CSpriteRenderer::GetSize(unsigned n, float& x, float& y){
  return GetSize(n, 0, x, y);
} //GetSize

/// Reader function for the width and height of a sprite frame.
/// \param n [in] Sprite index, assumed to be in range.
/// \param m [in] Frame number, assumed to be in range.
/// \param x [out] Width of frame in pixels.
/// \param y [out] Height of frame in pixels.

void CSpriteRenderer::GetSize(unsigned n, unsigned m, float& x, float& y){
  if(m_pSprite[n] == nullptr)
    x = y = 0;
  else m_pSprite[n]->GetSize(m, x, y);
} //GetSize

/// Reader function for the height of frame zero of a sprite.
/// \param n Sprite index, assumed to be in range.
/// \return Height in pixels.

float CSpriteRenderer::GetHeight(unsigned n){
  return m_pSprite[n]? m_pSprite[n]->GetHeight(): 0;
} //GetHeight

/// Reader function for the width of frame zero of a sprite.
/// \param n Sprite index, assumed to be in range.
/// \return Width in pixels.

float CSpriteRenderer::GetWidth(unsigned n){
  return m_pSprite[n]? m_pSprite[n]->GetWidth(): 0;
} //GetWidth

/// AABB intersect view frustum test.
/// \param box An AABB.
/// \return true if the box overlaps the view frustum of the camera.

bool CSpriteRenderer::BoxInFrustum(const BoundingBox& box){
  return m_pCamera->BoxInFrustum(box);
} //BoxInFrustum




