#ifndef MAP_DRAWING
#define MAP_DRAWING

#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <png.h>

// This takes the float value 'val', converts it to red, green & blue values, then 
// sets those values into the image memory buffer location pointed to by 'ptr'
inline void setRGB(png_byte *ptr, int val);

// This function actually writes out the PNG image file. The string 'title' is
// also written into the image file
int writeImage(const char* filename, int width, int height, float *heightmap, const char* title);

int writeImageColors(const char* filename, int width, int height, float *heightmap, const char* title);

inline void setRGB(png_byte *ptr, int val)
{
  ptr[0] = val;
  ptr[1] = val;
  ptr[2] = val;
}

inline void setColor(png_byte *ptr, png_byte r, png_byte g, png_byte b)
{
  ptr[0] = r;
  ptr[1] = g;
  ptr[2] = b;
}


#endif
