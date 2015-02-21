#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <png.h>

#include "noise.hpp"
#include "sqrdmd.hpp"
#include "simplerandom.hpp"
#include "map_drawing.hpp"

void produce_image(float* heightmap, int width, int height, const char* filename)
{
    writeImage((char*)filename, width, height, heightmap, "FOO");
}

void generate(long seed, float* heightmap, int width, int height)
{
  SimpleRandom randsource(seed);
  memset(heightmap, 0, sizeof(float) * width * height);
  createNoise(heightmap, WorldDimension(width, height), randsource, false);
  normalize(heightmap, width * height);
}

int main(int argc, char* argv[])
{
    int seed = 10;
    printf("Generating adapted_sqrdmd_rect noise with seed %d\n", seed);

    int width = 800;
    int height = 600;

    float heightmap_large[width * height];
    generate(seed, heightmap_large, width, height);

    produce_image(heightmap_large, width, height, "adapted_sqrdmd_rect.png");
}
