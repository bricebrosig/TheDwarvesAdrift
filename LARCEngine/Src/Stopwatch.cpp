/// \file Stopwatch.cpp
/// \brief Code for the stopwatch class CStopwatch.

#include "Stopwatch.h"

/// Get the time showing on the stopwatch.
/// We need to be careful here that we return the
/// correct value even when the stopwatch is stopped.
/// (Even when the stopwatch is stopped, the timer keeps ticking.)
/// \return Number of seconds on the stopwatch.

float CStopwatch::time(){
  if(m_bStopped && m_fStoppedTime > m_fStartTime) //stopped
    return m_fStoppedTime - m_fStartTime; //time from start to stop

  else{ //running
    const float t = CTimer::time(); //current time
    if(t > m_fStartTime) //to be safe
      return t - m_fStartTime; //time from start to now
    else return 0.0f; //should never happen
  } //else
} //time

/// Start the stopwatch at the current time.

void CStopwatch::start(){
  m_bStopped = false;
   CTimer::start();
} //start

/// Stop the stopwatch at the current time.

void CStopwatch::stop(){
  m_bStopped = true;
  m_fStoppedTime = CTimer::time();
} //stop

/// The elapsed function is a useful function for measuring repeating time intervals.
/// Given the start and duration times, this function returns TRUE if the interval
/// is over, and has the side-effect of resetting the start time when that happens,
/// thus setting things up for the next interval.
/// \param start Start of time interval.
/// \param interval Duration of interval.
/// \return TRUE if time interval is over.

bool CStopwatch::elapsed(float &start, float interval){
  const float t = time();

  if(t >= start + interval){ //if interval is over
    start = t; //reset start 
    return true; //signal time period has elapsed
  } //if

  else return false; //otherwise, do not signal
} //elapsed