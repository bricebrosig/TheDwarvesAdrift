/// \file BaseCamera.h
/// \brief Code for the base camera class CBaseCamera.

#include "BaseCamera.h"
#include "ComponentIncludes.h"
#include "Helpers.h"

/// Set the orientation, view, and projection matrices to the identity matrix.

CBaseCamera::CBaseCamera(){
  XMStoreFloat4x4(&m_orient, XMMatrixIdentity());
  XMStoreFloat4x4(&m_view, XMMatrixIdentity());
  XMStoreFloat4x4(&m_projection, XMMatrixIdentity());
} //constructor

/// \param aspectratio Window aspect ratio.
/// \param hfov Horizontal field of view angle
/// \param nearz Z coordinate of the near clipping plane.
/// \param farz Z coordinate of the far clipping plane.

void CBaseCamera::SetPerspective(float aspectratio, float hfov, float nearz, float farz){
  XMMATRIX project = XMMatrixPerspectiveFovLH(hfov/aspectratio, aspectratio, nearz, farz);
  XMStoreFloat4x4(&m_projection, project);
} //SetPerspective

/// \param width Window width in pixels.
/// \param ht Window height in pixels.
/// \param nearz Z coordinate of the near clipping plane.
/// \param farz Z coordinate of the far clipping plane.

void CBaseCamera::SetOrthographic(float width, float ht, float nearz, float farz){
  XMMATRIX project = XMMatrixOrthographicLH(width, ht, nearz, farz);
  XMStoreFloat4x4(&m_projection, project);
} //SetOrthographic

/// Move the camera along its velocity vector for a given
/// amout of time.
/// \param t Frame time in seconds.

void CBaseCamera::Move(float t){
  m_vPos += Vector3::Transform(t*m_vVel, XMLoadFloat4x4(&m_orient)); 
  Update();
} //move

/// \param p New position.

void CBaseCamera::MoveTo(const Vector3& p){
  m_vPos = p;
  Update();
} //move

/// \param angle Angle to be added to yaw, in radians.

void CBaseCamera::AddToYaw(float angle){
  m_fYaw += angle;
  Update();
} //AddToYaw

/// \param angle Angle to be added to pitch, in radians.

void CBaseCamera::AddToPitch(float angle){
  m_fPitch += angle;
  Update();
} //AddToPitch

/// \param angle Angle to be added to roll, in radians.

void CBaseCamera::AddToRoll(float angle){
  m_fRoll += angle;
  Update();
} //AddToRoll

/// \param angle New yaw angle, in radians.

void CBaseCamera::SetYaw(float angle){
  m_fYaw = angle;
  Update();
} //AddToYaw

/// \param angle New pitch angle, in radians.

void CBaseCamera::SetPitch(float angle){
  m_fPitch = angle;
  Update();
} //AddToPitch

/// \param angle New roll angle, in radians.

void CBaseCamera::SetRoll(float angle){
  m_fRoll = angle;
  Update();
} //AddToRoll

/// Reader function for the yaw.
/// \return angle Yaw in radians.

float CBaseCamera::GetYaw(){
  return m_fYaw;
} //GetYaw

/// Reader function for the pitch.
/// \return angle Pitch in radians.

float CBaseCamera::GetPitch(){
  return m_fPitch;
} //GetPitch

/// Reader function for the roll.
/// \return angle Roll in radians.

float CBaseCamera::GetRoll(){
  return m_fRoll;
} //GetRoll

/// Update the camera's orientation matrix, view vector, up vector,
/// view matrix, and view frustum from its current yaw, pitch, roll,
/// and position.

void CBaseCamera::Update(){
  const float alpha = XM_PIDIV2 - 0.01f;
  m_fPitch = clamp(-alpha, m_fPitch, alpha);

  XMMATRIX orient; //new orientation matrix
  orient = XMMatrixRotationZ(m_fRoll)*XMMatrixRotationX(m_fPitch)*XMMatrixRotationY(m_fYaw);
  XMStoreFloat4x4(&m_orient, orient);

  m_vView = Vector3::Transform(Vector3::UnitZ, orient);
  m_vUp = Vector3::Transform(Vector3::UnitY, orient);  
  XMStoreFloat4x4(&m_view, XMMatrixLookToLH(m_vPos, m_vView, m_vUp));

  BuildViewFrustum();
} //Update

/// Compute the view frustum from the camera's projection and view matrices.

void CBaseCamera::BuildViewFrustum(){
  BoundingFrustum::CreateFromMatrix(m_frustum, XMLoadFloat4x4(&m_projection)); //frustum in camera space

  XMMATRIX view = XMLoadFloat4x4(&m_view); //view matrix
  XMVECTOR determinant; //to check for invertibility
  XMMATRIX viewinv = XMMatrixInverse(&determinant, view); //inverse of view matrix

  if(XMVectorGetByIndex(determinant, 0) != 0.0f) //view matrix was invertible
    m_frustum.Transform(m_frustum, viewinv); //transform frustum into world space
} //BuildViewFrustum

/// \param box An AABB.
/// \return true if the AABB intersects the view frustum.

bool CBaseCamera::BoxInFrustum(const BoundingBox& box){
  return m_frustum.Intersects(box);
} //BoxInFrustum

/// \param p The new velocity vector.

void CBaseCamera::SetVel(const Vector3& p){
  m_vVel = p;
} //SetVel

/// Reader function for the view vector.
/// \return The view vector.

const Vector3& CBaseCamera::GetViewVector(){
  return m_vView;
} //GetViewVector

/// Reader function for the view frustum.
/// \return The view frustum.

const BoundingFrustum& CBaseCamera::GetFrustum(){
  return m_frustum;
} //GetFrustum

/// Reader function for the camera position.
/// \return Camera position.

const Vector3& CBaseCamera::GetPos(){
  return m_vPos;
} //GetPos
