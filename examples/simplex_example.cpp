#include <stdio.h>
#include "sqrdmd.hpp"
#include "simplexnoise.hpp"
#include <cstdlib>
#include <cstring>
#include "map_drawing.hpp"

void produce_image(float* heightmap, int width, int height, const char* filename)
{
    writeImage((char*)filename, width, height, heightmap, "FOO");
}

void generate(long seed, float* heightmap, int width, int height)
{
    memset(heightmap, 0, sizeof(float) * width * height);
    simplexnoise(seed, heightmap, width, height, 0.5f);
    normalize(heightmap, width * height);
}

int main(int argc, char* argv[])
{
    int seed = 10;
    printf("Generating a simplex noise map with seed %d\n", seed);

    float heightmap_square[513 * 513];
    generate(seed, heightmap_square, 513, 513);

    float heightmap_large[513 * 129];
    generate(seed, heightmap_large, 513, 129);

    float heightmap_tall[129 * 513];
    generate(seed, heightmap_tall, 129, 513);

    produce_image(heightmap_square, 513, 513, "simplex_square.png");
    produce_image(heightmap_large, 513, 129, "simplex_large.png");
    produce_image(heightmap_tall, 129, 513, "simplex_tall.png");
}