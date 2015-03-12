#include "map_drawing.hpp"
#include "utils.hpp"
#include <stdexcept>

using namespace std;

int writeImage(const char* filename, int width, int height, float *heightmap, const char* title)
{
  int code = 0;
  FILE *fp;
  png_structp png_ptr;
  png_infop info_ptr;
  png_bytep row;
  
  // Open file for writing (binary mode)
  fp = fopen(filename, "wb");
  if (fp == NULL) {
    fprintf(stderr, "Could not open file %s for writing\n", filename);
    code = 1;
    goto finalise;
  }

  // Initialize write structure
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fprintf(stderr, "Could not allocate write struct\n");
    code = 1;
    goto finalise;
  }

  // Initialize info structure
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    fprintf(stderr, "Could not allocate info struct\n");
    code = 1;
    goto finalise;
  }

  // Setup Exception handling
  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "Error during png creation\n");
    code = 1;
    goto finalise;
  }

  png_init_io(png_ptr, fp);

  // Write header (8 bit colour depth)
  png_set_IHDR(png_ptr, info_ptr, width, height,
      8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  // Set title
  if (title != NULL) {
    png_text title_text;
    title_text.compression = PNG_TEXT_COMPRESSION_NONE;
    title_text.key = "Title";
    title_text.text = (char*)title;
    png_set_text(png_ptr, info_ptr, &title_text, 1);
  }

  png_write_info(png_ptr, info_ptr);

  // Allocate memory for one row (3 bytes per pixel - RGB)
  row = (png_bytep) malloc(3 * width * sizeof(png_byte));

  // Write image data
  int x, y;
  for (y=0 ; y<height ; y++) {
    for (x=0 ; x<width ; x++) {

      float h = heightmap[(y*width + x)];          
      float res = 0.0f;
      if (h <= 0.0f) {
        res = 0;
      } else if (h >= 1.0f) {
        res = 255;
      } else {
        res = (h * 255.0f);
      }

      setRGB(&(row[x*3]), res);
    }
    png_write_row(png_ptr, row);
  }



          

  // End write
  png_write_end(png_ptr, NULL);

  finalise:
  if (fp != NULL) fclose(fp);
  if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
  if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
  if (row != NULL) free(row);

  return code;
}


float find_value_for_quantile(const float quantile, const float* array, const uint32_t size)
{
    float value = 0.5;
    float th_step = 0.5;

    while (th_step > 0.00001)
    {
        uint32_t count = 0;
        for (uint32_t i = 0; i < size; ++i)
            count += (array[i] < value);

        th_step *= 0.5;
        if (count / (float)size < quantile)
            value += th_step;
        else
            value -= th_step;
    }
    return value;
}

void gradient(png_byte *ptr, png_byte ra, png_byte ga, png_byte ba, png_byte rb, png_byte gb, png_byte bb, float h, float ha, float hb)
{
  if (ha>hb) {
    printf("BAD1\n");
    throw runtime_error("BAD1\n");
  }
  if (hb<h) {
    printf("BAD2\n");
    throw runtime_error("BAD2\n");
  }
  if (ha>h) {
    printf("BAD3\n");
    throw runtime_error("BAD3\n");
  }
  float h_delta = hb - ha;
  float simil_b = (h - ha)/h_delta;
  float simil_a = (1.0f - simil_b);
  setColor(ptr, 
    (float)simil_a * ra + (float)simil_b * rb,
    (float)simil_a * ga + (float)simil_b * gb,
    (float)simil_a * ba + (float)simil_b * bb);
}

int writeImageColors(const char* filename, int width, int height, float *heightmap, const char* title)
{
  float q15, q70, q75, q90, q95, q99;
  int code = 0;
  FILE *fp;
  png_structp png_ptr;
  png_infop info_ptr;
  png_bytep row;
  
  // Open file for writing (binary mode)
  fp = fopen(filename, "wb");
  if (fp == NULL) {
    fprintf(stderr, "Could not open file %s for writing\n", filename);
    code = 1;
    goto finalise;
  }

  // Initialize write structure
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fprintf(stderr, "Could not allocate write struct\n");
    code = 1;
    goto finalise;
  }

  // Initialize info structure
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    fprintf(stderr, "Could not allocate info struct\n");
    code = 1;
    goto finalise;
  }

  // Setup Exception handling
  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "Error during png creation\n");
    code = 1;
    goto finalise;
  }

  png_init_io(png_ptr, fp);

  // Write header (8 bit colour depth)
  png_set_IHDR(png_ptr, info_ptr, width, height,
      8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  // Set title
  if (title != NULL) {
    png_text title_text;
    title_text.compression = PNG_TEXT_COMPRESSION_NONE;
    title_text.key = "Title";
    title_text.text = (char*)title;
    png_set_text(png_ptr, info_ptr, &title_text, 1);
  }

  png_write_info(png_ptr, info_ptr);

  // Allocate memory for one row (3 bytes per pixel - RGB)
  row = (png_bytep) malloc(3 * width * sizeof(png_byte));

  // Write image data  
  q15 = find_value_for_quantile(0.15f, heightmap, width * height);
  q70 = find_value_for_quantile(0.70f, heightmap, width * height);
  q75 = find_value_for_quantile(0.75f, heightmap, width * height);
  q90 = find_value_for_quantile(0.90f, heightmap, width * height);
  q95 = find_value_for_quantile(0.95f, heightmap, width * height);
  q99 = find_value_for_quantile(0.99f, heightmap, width * height);
  /*printf(" * value for q .15 %f\n", q15);
  printf(" * value for q .65 %f\n", q65);
  printf(" * value for q .90 %f\n", q90);
  printf(" * value for q .95 %f\n", q95);
  printf(" * value for q .99 %f\n", q99);*/

  int x, y;
  for (y=0 ; y<height ; y++) {
    for (x=0 ; x<width ; x++) {

      float h = heightmap[(y*width + x)];          
      float res = 0.0f;

      if (h < q15) {
        gradient(&(row[x*3]), 0, 0, 255, 0, 20, 200, h, 0.0f, q15);                 
        continue;
      }

      if (h < q70) {
        gradient(&(row[x*3]), 0, 20, 200, 50, 80, 225, h, q15, q70);                 
        continue;
      }

      if (h < q75) {
        gradient(&(row[x*3]), 50, 80, 225, 135, 237, 235, h, q70, q75);
        continue;
      }

      if (h < q90) {
        gradient(&(row[x*3]), 88, 173, 49, 218, 226, 58, h, q75, q90);
        //setColor(&(row[x*3]), 20, 240, 20);                  
        continue;
      }

      if (h < q95) {
        gradient(&(row[x*3]), 218, 226, 58, 251, 252, 42, h, q90, q95);
        //setColor(&(row[x*3]), 215, 219, 137);                  
        continue;
      }

      if (h < q99) {
        gradient(&(row[x*3]), 251, 252, 42, 91, 28, 13, h, q95, q99);
        //setColor(&(row[x*3]), 219, 185, 137);                  
        continue;
      }


      gradient(&(row[x*3]), 91, 28, 13, 51, 0, 4, h, q99, 1.0f);

      if (h <= 0.0f) {
        res = 0;        
      } else if (h >= 1.0f) {
        res = 255;
      } else {
        res = (h * 255.0f);
      }      

      setRGB(&(row[x*3]), res);
    }
    png_write_row(png_ptr, row);
  }



          

  // End write
  png_write_end(png_ptr, NULL);

  finalise:
  if (fp != NULL) fclose(fp);
  if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
  if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
  if (row != NULL) free(row);

  return code;
}
