#include <stdio.h>
#include "sqrdmd.hpp"
#include "simplexnoise.hpp"
#include <cstdlib>
#include <cstring>
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
    width  = ilGetInteger(IL_IMAGE_WIDTH) / 2;
    height = ilGetInteger(IL_IMAGE_HEIGHT) / 2;

    for (int y = 0; y < height; y++)
    {
       for (int x = 0; x < width; x++)
       {
          float h = heightmap[(y*width + x)];          
          float res = 0.0f;
          if (h <= 0.0f) {
            res = 0;
          } else if (h >= 1.0f) {
            res = 255;
          } else {
            res = (ILubyte)(h * 255.0f);
          }
          //printf("h= %f res=%i\n", h, res);
          bytes[(y+0)*width*2 + (x+0)] = res;
          bytes[(y+0)*width*2 + (x+width)] = res;
          bytes[(y+height)*width*2 + (x+0)] = res;
          bytes[(y+height)*width*2 + (x+width)] = res;
       }
    }
}

void save_image(float* heightmap, int width, int height, const char* filename)
{
    ILuint imageName = GenerateSingleImage();    
    CheckForErrors();
    ilBindImage(imageName);    
    CheckForErrors();
    iluScale(width * 2, height * 2, 32);
    CheckForErrors();

    drawMap(heightmap);   

    ilEnable(IL_FILE_OVERWRITE);
    CheckForErrors();
    ilSaveImage(filename); 
    CheckForErrors();
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

    ilInit();
    iluInit();

    save_image(heightmap_square, 513, 513, "simplex_square.png");
    save_image(heightmap_large, 513, 129, "simplex_large.png");
    save_image(heightmap_tall, 129, 513, "simplex_tall.png");
}