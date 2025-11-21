#include <stdio.h>
#include "platecapi.hpp"
#include "sqrdmd.hpp"
#include <cstdlib>
#include "map_drawing.hpp"
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <utils.hpp>

#include <execinfo.h>

void produce_image_gray(float* heightmap, int width, int height, const char* filename)
{
    writeImageGray((char*)filename, width, height, heightmap, "FOO");
}

void produce_image_colors(float* heightmap, int width, int height, const char* filename)
{
    writeImageColors((char*)filename, width, height, heightmap, "FOO");
}

void save_image(void* p, const char* filename, const int width, const int height, bool colors)
{
    const float* heightmap = platec_api_get_heightmap(p);
    float* copy = new float[width * height];
    memcpy(copy, heightmap, sizeof(float) * width * height);
    normalize(copy, width * height);

    if (colors)
        produce_image_colors(copy, width, height, filename);
    else
        produce_image_gray(copy, width, height, filename);
    delete[] copy;
}

typedef struct {
    uint32_t seed;
    uint32_t width;
    uint32_t height;
    bool colors;
    char* filename;
    uint32_t step;
} Params;

char DEFAULT_FILENAME[] = "simulation";

void fill_params(Params& params, int argc, char* argv[])
{
    srand(time(nullptr));
    params.seed = rand();
    params.width = 600;
    params.height = 400;
    params.colors = true;
    params.filename = DEFAULT_FILENAME;
    params.step = 0;

    int p = 1;
    while (p < argc) {
        if (0 == strcmp(argv[p], "--help") || 0 == strcmp(argv[p], "-h")) {
            printf(" -h --help           : show this message\n");
            printf(" -s SEED             : use the given SEED\n");
            printf(" --dim WIDTH HEIGHT  : use the given width and height\n");
            printf(" --colors            : generate a colors map\n");
            printf(" --grayscale         : generate a grayscale map\n");
            printf(" --filename FILENAME : generated map are named with the given filename (the extension is appended)\n");
            printf(" --step X            : generate intermediate maps any given steps\n");
            exit(0);
        } else if (0 == strcmp(argv[p], "-s")) {
            if (p + 1 >= argc) {
                printf("error: a parameter should follow -s\n");
                exit(1);
            }
            long seed = atol(argv[p+1]);
            if (seed==0) {
                printf("error: not a number\n");
                exit(1);
            }
            params.seed = seed;
            p += 2;
        } else if (0 == strcmp(argv[p], "--dim")) {
            if (p + 2 >= argc) {
                printf("error: two parameters should follow --dim\n");
                exit(1);
            }
            int width = atoi(argv[p+1]);
            int height = atoi(argv[p+2]);
            if (width==0 || height==0) {
                printf("error: not a number\n");
                exit(1);
            }
            if (width<5 || height<5) {
                printf("error: dimensions have to be positive and >= 5\n");
                exit(1);
            }
            params.width = width;
            params.height = height;
            p += 3;
        } else if (0 == strcmp(argv[p], "--colors")) {
            params.colors = true;
            p += 1;
        } else if (0 == strcmp(argv[p], "--grayscale")) {
            params.colors = false;
            p += 1;
        } else if (0 == strcmp(argv[p], "--filename")) {
            if (p + 1 >= argc) {
                printf("error: a parameter should follow --filename\n");
                exit(1);
            }
            params.filename = argv[p+1];
            p += 2;
        } else if (0 == strcmp(argv[p], "--step")) {
            if (p + 1 >= argc) {
                printf("error: a parameter should follow --step\n");
                exit(1);
            }
            int step = atoi(argv[p+1]);
            if (step==0) {
                printf("error: not a number\n");
                exit(1);
            }
            if (step<0) {
                printf("error: step have to be positive\n");
                exit(1);
            }
            params.step = step;
            p += 2;
        } else {
            printf("Unexpected param '%s' use -h to display a list of params\n", argv[p]);
            exit(1);
        }
    }
}

/// Should take several parameters:
/// - colors/grayscale
/// - width and height
/// - if to generate the intermediate imaged and how frequently
/// - the output filename
/// - the seed
int main(int argc, char* argv[])
{
    Params params;
    fill_params(params, argc, argv);

    printf("Plate-tectonics simulation example\n");
    printf(" seed     : %u\n", params.seed);
    printf(" width    : %u\n", params.width);
    printf(" height   : %u\n", params.height);
    printf(" map      : %s\n", params.colors ? "colors" : "grayscale");
    printf(" filename : %s\n", params.filename);
    if (params.step == 0)
        printf(" step     : no\n");
    else
        printf(" step     : %u\n", params.step);

    printf("\n");

    void* p = platec_api_create(params.seed, params.width, params.height, 0.65, 60, 0.02,1000000, 0.33, 2, 10);

    char filenamei[250];
    snprintf(filenamei, sizeof(filenamei), "%s_initial.png", params.filename);
    save_image(p, filenamei, params.width, params.height, params.colors);
    printf(" * initial map created\n");

    int step = 0;
    while (platec_api_is_finished(p) == 0) {
        step++;
        platec_api_step(p);

        if (params.step != 0 && (step % params.step == 0) ) {
            char filename[250];
            snprintf(filename, sizeof(filename), "%s_%i.png", params.filename, step);
            printf(" * step %i (filename %s)\n", step, filename);
            save_image(p, filename, params.width, params.height, params.colors);
        }
    }

    char filename[250];
    snprintf(filename, sizeof(filename), "%s.png", params.filename);
    save_image(p, filename, params.width, params.height, params.colors);
    printf(" * simulation completed (filename %s)\n", filename);
}