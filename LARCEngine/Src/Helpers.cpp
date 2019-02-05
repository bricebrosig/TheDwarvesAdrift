/// \file Helpers.cpp
/// \brief Code file for helper functions.

#include "Helpers.h"

/// Helper function to make a wide file name out of a regular one.
/// Creates a wide buffer and stores the wide version of a regular
/// file name in it. Remember to delete the wide file name after
/// it has been used.
/// \param filename [in] The original file name.
/// \param wfilename [out] The wide file name.

void MakeWideFileName(_In_ const char* filename, _Out_ wchar_t*& wfilename){
  const size_t newsize = strlen(filename) + 1;  
  wfilename = new wchar_t[newsize];
  size_t n; //unused
  mbstowcs_s(&n, wfilename, newsize, filename, _TRUNCATE); 
} //MakeWideFileName

/// Convert color in HSV format to RGB format. This
/// is a helper function for generating a pseudorandom color.
/// I snagged it from the interwebs.
/// \param h Hue.
/// \param s Saturation.
/// \param v Value.
/// \return Color in RGB format with an alpha value of 1.

XMFLOAT4 HSVtoRGB(float h, float s, float v){
  const unsigned i = unsigned(6.0f*h);

  const float f = 6.0f*h - i;
  const float p = v*(1.0f - s);
  const float q = v*(1.0f - s*f);
  const float t = v*(1.0f - s*(1.0f - f));

  float r, g, b; //RGB values

  switch(i%6){ //set r, g, b in different ways depending on the magnitude of h
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  } //switch
  
  return XMFLOAT4(r, g, b, 1.0f); //return as an XMFLOAT4 with alpha channel 1.0.
} //HSVtoRGB