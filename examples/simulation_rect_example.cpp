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
#include <stdexcept>

#include <execinfo.h>


void handler() {
  printf("\nException caused failure\n\n");
  void *trace_elems[20];
    int trace_elem_count(backtrace( trace_elems, 20 ));
    char **stack_syms(backtrace_symbols( trace_elems, trace_elem_count ));
    for ( int i = 0 ; i < trace_elem_count ; ++i )
    {
        std::cout << stack_syms[i] << "\n";
    }
    free( stack_syms );
  exit(1);
}

void produce_image(float* heightmap, int width, int height, const char* filename)
{
    writeImage((char*)filename, width, height, heightmap, "FOO");
}

int main(int argc, char* argv[])
{
    //std::set_terminate( handler );

    int seed = 10;
    int width = 800;
    int height = 600;
    printf("Generating a map with seed %d\n", seed);

    void* p = platec_api_create(seed, width, height, 0.65, 60, 0.02,1000000, 0.33, 2, 10);
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
    float copy[width * height];
    memcpy(copy, heightmap, sizeof(float)*width*height);
    printf(" * heightmap obtained\n");

    //normalize(copy, width * height);

    produce_image(copy, width, height, "simulation_rect.png");
}