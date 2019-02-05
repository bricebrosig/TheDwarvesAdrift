/// \file timer.cpp
/// \brief Code for timer class CTimer.

#include <windows.h> //for timeGetTime()
#pragma comment(lib, "winmm.lib") //for timeGetTime()

#include "timer.h"

CTimer::CTimer(){
  m_nStartTime = timeGetTime();
} //constructor

/// Initialize the timer by querying the performance counter
/// for the current time and saving it in m_lnStartTime.

void CTimer::start(){ 
  if(m_bStarted)return; //bail out
  m_nStartTime = timeGetTime();
  m_bStarted = true;
} //start

/// Get elapsed time from when the timer was started to
/// the start of the current frame, in seconds.
/// \return Time from timer start to start of frame in seconds.

float CTimer::time(){ 
  return m_fFrameStartTime;
} //time

/// Get current time since the timer was started.
/// \return Current time since start in seconds.


/// Get elapsed time from when the timer was started to
/// now, in seconds.
/// \return Time from timer start to now.

float CTimer::actualtime(){ 
  return (timeGetTime() - m_nStartTime)/1000.0f;
} //time

/// Get current time in raw form from the performance counter.
/// This function is intended to be used to generate a seed for a PRNG.
/// \return Current time in raw form.

unsigned CTimer::rawtime(){ 
  return timeGetTime();
} //rawtime

/// Reader function for the frame time in seconds.
/// \return Duration of previous frame.

float CTimer::frametime(){ 
  return m_fFrameTime; 
} //frametime

/// Reader function for the frame rate in frames per second.
/// \return The current frame rate.

unsigned CTimer::framerate(){
  return m_nFrameRate;
} //framerate

/// The elapsed function is a useful function for measuring repeating time
/// intervals. Given the start and duration times, this function returns
/// true if the interval is over, and has the side-effect of resetting 
/// the start time when that happens, thus setting things up for the 
/// next interval.
/// \param start [in, out] Start of repeating time interval, in seconds.
/// \param interval Duration of interval in seconds.
/// \return true if time interval is over.

bool CTimer::elapsed(float &start, float interval){
  if(!m_bStarted)return false; //bail and fail

  if(m_fFrameStartTime >= start + interval){ //if interval has elapsed
    start = m_fFrameStartTime; //reset start for next interval
    return true; //signal that time period has elapsed
  } //if
  else return false; //otherwise, do not signal
} //elapsed

/// This must be called before each animation frame
/// to record the time that it started.

void CTimer::BeginFrame(){ 
  m_fFrameStartTime = actualtime();
} //BeginFrame

/// This must be called after each animation frame to
/// compute the frame time and the frame rate. 

void CTimer::EndFrame(){
  if(!m_bStarted)start(); //start the timer on the first frame

  ++m_nFrameCount; //one more frame for frame rate
  m_fFrameTime = actualtime() - m_fFrameStartTime; //frame time

  if(elapsed(m_fStartFrameRate, 1.0f)){ //adjust frame rate each second
    m_nFrameRate = m_nFrameCount;
    m_nFrameCount = 0;
  } //if

  m_fFrameTime = min(m_fFrameTime, 0.033333f); //even out big bumps
} //EndFrame
