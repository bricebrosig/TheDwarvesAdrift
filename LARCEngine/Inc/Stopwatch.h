/// \file Stopwatch.h
/// \brief Interface for the stopwatch class CStopwatch.

#pragma once

#include "Timer.h"

/// \brief A stopwatch.
///
/// CStopwatch is used to time things like you
/// would a physical stopwatch. 

class CStopwatch: public CTimer{
  private:
    float m_fStartTime = 0.0f; ///< Time started.
    float m_fStoppedTime = 0.0f; ///< Time stopped.
    bool m_bStopped = true; ///< Whether time stopped.

  public:
    void start(); ///< Start timing from zero.
    void stop(); ///< Stop timing.
    float time(); ///< Get elapsed time in seconds.
    bool elapsed(float &start, float interval); ///< Has interval elapsed since start?
}; //CStopwatch
