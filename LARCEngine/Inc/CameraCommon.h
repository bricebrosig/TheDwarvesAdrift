/// \file CameraCommon.h
/// \brief Interface for the camera common variables class CCameraCommon.

#pragma once

#include "D3DDefines.h"

/// \brief The camera common variables class.
///
/// CCameraCommon is a singleton class that encapsulates 
/// camera properties that need to be shared between classes,
/// specifically, the orientation, view, and projection matrices
/// in a form that doesn't need to be 16-byte aligned. Byte alignment
/// is necessary for things that need to be shared with the GPU. 
/// Don't ignore it - if you do then things might appear to work for a while,
/// but it will bite you in the neck when you least expect it.

class CCameraCommon{
  protected: //the following are matrices in a form that doesn't need to be 16-byte aligned  
    static XMFLOAT4X4 m_orient; ///< Orientation matrix.
    static XMFLOAT4X4 m_view; ///< View matrix.
    static XMFLOAT4X4 m_projection; ///< Projection matrix.
}; //CCameraCommon
