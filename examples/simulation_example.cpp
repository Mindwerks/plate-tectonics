#include <stdio.h>
#include "platecapi.hpp"
#include "sqrdmd.hpp"
#include <cstdlib>
#include "map_drawing.hpp"

void produce_image(float* heightmap, int width, int height, const char* filename)
{
    writeImage((char*)filename, width, height, heightmap, "FOO");
}

int main(int argc, char* argv[])
{
    int seed = 10;
    printf("Generating a map with seed %d\n", seed);

    void* p = platec_api_create(seed, 512, 512, 0.65,60,0.02,1000000,0.33,2,10);
    printf(" * initial map created\n");

    int step = 0;
    while (platec_api_is_finished(p) == 0) {
        step++;
        platec_api_step(p);

        if (step % 50 == 0) {
            printf(" * step %i\n", step);
        }
    }

    printf(" * simulation completed\n");
    const float* heightmap = platec_api_get_heightmap(p);
    float copy[512 * 512];
    memcpy(copy, heightmap, sizeof(float)*512*512);
    printf(" * heightmap obtained\n");


    normalize(copy, 512 * 512);

    produce_image(copy, 512, 512, "simulation.png");
}