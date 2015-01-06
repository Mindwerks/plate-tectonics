#include <stdio.h>
#include "platecapi.hpp"
#include "sqrdmd.hpp"
#include <cstdlib>
#include <il.h>
#include <ilu.h>

ILuint GenerateSingleImage(void) 
{
    ILuint ImageName; // The image name to return.
    ilGenImages(1, &ImageName); // Grab a new image name.
    return ImageName; // Go wild with the return value.
} 

void CheckForErrors()
{
    ILenum Error;
    while ((Error = ilGetError()) != IL_NO_ERROR) { 
        printf("%d/n", Error); 
        exit(1);
    } 
}

void drawMap(const float* heightmap)
{
    ILubyte* bytes = ilGetData(); 
    ILuint width,height;
    width  = ilGetInteger(IL_IMAGE_WIDTH);
    height = ilGetInteger(IL_IMAGE_HEIGHT);

    for (int y = 0; y < height; y++)
    {
       for (int x = 0; x < width; x++)
       {
          float h = heightmap[(y*width + x)];
          if (h <= 0.0f) {
            bytes[(y*width + x)] = 0;
          } else if (h >= 1.0f) {
            bytes[(y*width + x)] = 255;
          } else {
            bytes[(y*width + x)] = (ILubyte)(h*255.0f);
          }          
       }
    }
}

int main(int argc, char* argv[])
{
    int seed = 10;
    int width = 1000;
    int height = 250;
    printf("Generating a map with seed %d\n", seed);

    void* p = platec_api_create(seed, width, height, 0.65,60,0.02,1000000,0.33,2,10);
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

    ilInit();
    iluInit();

    ILuint imageName = GenerateSingleImage();    
    CheckForErrors();
    ilBindImage(imageName);    
    CheckForErrors();
    iluScale(width, height, 32);
    CheckForErrors();

    normalize(copy, width * height);
    drawMap(copy);   

    ilEnable(IL_FILE_OVERWRITE);
    CheckForErrors();
    ilSaveImage("map_rect.png"); 
    CheckForErrors();
}