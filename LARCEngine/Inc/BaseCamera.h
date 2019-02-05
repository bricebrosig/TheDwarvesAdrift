/// \file BaseCamera.h
/// \brief Interface for the base camera class CBaseCamera.

#pragma once

#include "CameraCommon.h"

/// \brief The base camera.
///
/// Basic camera functionality including position, orientation,
/// velocity, and frustum culling. 

class CBaseCamera: public CCameraCommon{
  protected: 
    Vector3 m_vPos = Vector3::Zero; ///< Position.
    Vector3 m_vVel = Vector3::Zero; ///< Velocity.

    float m_fYaw = 0.0f; ///< Yaw angle.
    float m_fPitch = 0.0f; ///< Pitch angle.
    float m_fRoll = 0.0f; ///< Roll angle.

    Vector3 m_vView = Vector3(0, 0, 1.0f); ///< View vector.
    Vector3 m_vUp = Vector3::UnitY; ///< Up vector.

	  BoundingFrustum m_frustum; ///< View frustum for frustum clipping.

    void BuildViewFrustum(); ///< Build the view frustum.
    virtual void Update(); ///< Update matrices and frustum.

  public:
    CBaseCamera(); ///< Constructor.

    void SetPerspective(float aspectratio, float hfov, float nearz, float farz); ///< Set perspective camera.
    void SetOrthographic(float width, float ht, float nearz, float farz); ///< Set orthographic camera.

    virtual void Move(float t); ///< Move along velocity vector.
    void MoveTo(const Vector3& p); ///< Move to new position.

    void SetVel(const Vector3& p); ///< Set velocity.

    void AddToYaw(float angle); ///< Add to yaw.
    void AddToPitch(float angle); ///< Add to pitch.
    void AddToRoll(float angle); ///< Add to roll.
    
    void SetYaw(float angle); ///< Set yaw.
    void SetPitch(float angle); ///< Set pitch.
    void SetRoll(float angle); ///< Set roll.
    
    float GetYaw(); ///< Get yaw.
    float GetPitch(); ///< Get pitch.
    float GetRoll(); ///< Get roll.

    bool BoxInFrustum(const BoundingBox& box); ///< Does the box overlap the view frustum?

    const Vector3& GetViewVector(); ///< Get the view vector.
    const BoundingFrustum& GetFrustum(); ///< Get the view frustum.
    const Vector3& GetPos(); ///< Get position.
}; //CBaseCamera
