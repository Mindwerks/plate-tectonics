#include <stdio.h>
#include "sqrdmd.hpp"
#include <cstdlib>
#include <cstring>
#include "map_drawing.hpp"

void produce_image(float* heightmap, int width, int height, const char* filename)
{
    writeImage((char*)filename, width, height, heightmap, "FOO");
}

void generate(long seed, float* heightmap, int side)
{
    memset(heightmap, 0, sizeof(float) * side * side);
    sqrdmd(seed, heightmap, side, 0.7f);
    normalize(heightmap, side * side);
}

int main(int argc, char* argv[])
{
    int seed = 10;
    printf("Generating a square-diamond noise map with seed %d\n", seed);

    float heightmap_square[513 * 513];
    generate(seed, heightmap_square, 513);

    produce_image(heightmap_square, 513, 513, "sqrdmd_square.png");
}