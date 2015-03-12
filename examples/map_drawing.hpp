#ifndef MAP_DRAWING
#define MAP_DRAWING

#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <png.h>

// This function actually writes out the PNG image file. The string 'title' is
// also written into the image file
int writeImageGray(const char* filename, int width, int height, float *heightmap, const char* title);

int writeImageColors(const char* filename, int width, int height, float *heightmap, const char* title);

#endif
