/// \file timer.h
/// \brief Interface for the timer class CTimer.

#pragma once

#include "Defines.h"

/// \brief The timer. 
///
/// The timer allows you to manage game events by duration, rather than
/// on a frame-by-frame basis. This simple version of the timer is based on
/// the Windows API function timeGetTime, which is notoriously inaccurate
/// but perfectly adequate for a simple game demo.

class CTimer{ 
  private:
    bool m_bStarted = false; ///< Whether timer has been started.

    //time variables
    unsigned m_nStartTime = 0; ///< When timer was started, in milliseconds.
    float m_fFrameStartTime = 0; ///< When current frame was started, in seconds.
    float m_fFrameTime = 0; ///< Frame time for the previous frame, in seconds.

    //frame rate variables
    unsigned m_nFrameRate = 0; ///< Frames per second.
    float m_fStartFrameRate = 0; ///< When frame rate counting for the current second began, in seconds.
    unsigned m_nFrameCount = 0; ///< Number of frames so far in this second.

  protected:
    void start(); ///< Start the timer.

  public:
    CTimer(); ///< Constructor.
    
    float time(); ///< Return the time in seconds at the start of the current frame.
    float actualtime(); ///< Return the time in seconds.
    unsigned rawtime(); ///< Return the raw time.
    bool elapsed(float &start, float interval); ///< Elapsed time for repeating events.
    
    virtual float frametime(); ///< Get the duration of the last frame.
    unsigned framerate(); ///< Get the frame rate.

    void BeginFrame(); ///< Beginning of frame.
    void EndFrame(); ///< End of frame.
}; //CTimer
