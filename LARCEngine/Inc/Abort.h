/// \file abort.h
/// \brief Header and define for the ABORT macro.

#pragma once

/// \brief The function that really does the aborting.
///
/// Terminate the program with a printf-like formatted error message. 
/// Normally we will call this function using the ABORT macro.

void reallyAbort(const char *fmt, ...);

/// \brief Wide character version of function reallyAbort.

void reallyAbortW(const wchar_t *fmt, ...);

/// \brief The ABORT macro.
///
/// This nasty looking macro uses a handy little trick to allow it to
/// appear to to take a variable number of arguments. This is what
/// you should actually call to abort.

#define ABORT (reallyAbort)

/// \brief Wide character version of the ABORT macro.

#define ABORTW (reallyAbortW)
