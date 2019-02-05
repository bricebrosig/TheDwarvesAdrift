/// \file Helpers.h
/// \brief Header file for helper functions.

#pragma once

#include "Defines.h"

/// \brief Safe delete of a pointer. 
///
/// Note that it is OK to delete a null pointer. Note the brackets and braces also.

#define SAFE_DELETE(p) {delete (p); (p) = nullptr;};

#define lerp(t, a, b) (a + t*(b - a)) ///< Linear interpolation.
#define spline3(t) (t * t * (3.0f - 2.0f*t)) ///< Cubic spline.
#define spline5(t) (t * t * t * (10.0f + 3.0f * t * (2.0f*t - 5.0f))) ///< Quintic spline.

#define clamp(a, b, c) min(max(a, b), c) ///< Clamp between two values.

void MakeWideFileName(_In_ const char* filename, _Out_ wchar_t*& wfilename); ///< Make wide file name from a narrow one.
XMFLOAT4 HSVtoRGB(float h, float s, float v); //< Convert color from HSV to RGB/