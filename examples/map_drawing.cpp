#include "map_drawing.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <iostream>
#include <cstring>

using namespace std;

inline void setGray(png_byte *ptr, int val)
{
    ptr[0] = static_cast<png_byte>(val);
    ptr[1] = static_cast<png_byte>(val);
    ptr[2] = static_cast<png_byte>(val);
}

inline void setColor(png_byte *ptr, png_byte r, png_byte g, png_byte b)
{
    ptr[0] = r;
    ptr[1] = g;
    ptr[2] = b;
}

int writeImage(const char* filename, int width, int height, float *heightmap, const char* title,
               void (drawFunction)(png_structp&, png_bytep&, int, int, float*))
{
    volatile int code = 0;
    FILE * volatile fp = nullptr;
    png_structp volatile png_ptr = nullptr;
    png_infop volatile info_ptr = nullptr;
    png_bytep volatile row = nullptr;
    size_t row_bytes = 0;  // Declare early to avoid goto issues
    size_t alloc_size = 0;  // Declare early to avoid goto issues

    // Open file for writing (binary mode)
#ifdef _WIN32
    // fopen_s doesn't accept volatile pointer, so use a non-volatile temporary
    FILE* fp_temp = nullptr;
    errno_t err = fopen_s(&fp_temp, filename, "wb");
    fp = fp_temp;
    if (err != 0 || fp == nullptr) {
#else
    fp = fopen(filename, "wb");
    if (fp == nullptr) {
#endif
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        code = 1;
        goto finalise;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }

    // Setup Exception handling
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4611)  // Interaction between setjmp and C++ object destruction is acceptable here
#endif
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto finalise;
    }
#ifdef _MSC_VER
#pragma warning(pop)
#endif

    png_init_io(png_ptr, fp);

    // Write header (8-bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // Set title
    if (title != nullptr) {
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = const_cast<char*>("Title");
        title_text.text = (char*)title;
        png_set_text(png_ptr, info_ptr, &title_text, 1);
    }

    png_write_info(png_ptr, info_ptr);

    // Allocate memory for one row (3 bytes per pixel - RGB)
    // Add extra padding to detect buffer overruns
    row_bytes = 3 * width * sizeof(png_byte);
    std::cout << "  [PNG] Allocating row buffer: " << row_bytes << " bytes for width=" << width << std::endl;

    // Allocate extra 16 bytes and fill with canary pattern
    alloc_size = row_bytes + 16;
    row = (png_bytep) malloc(alloc_size);

    if (row == nullptr) {
        std::cerr << "  [PNG] ERROR: Failed to allocate row buffer of " << alloc_size << " bytes!" << std::endl;
        code = 1;
        goto finalise;
    }

    // Set canary bytes at the end
    memset(row + row_bytes, 0xCD, 16);
    std::cout << "  [PNG] Row buffer allocated successfully at " << static_cast<void*>(row) << std::endl;

    // Write image data
    // Need to create non-volatile references for function calls
    {
        std::cout << "  [PNG] Calling draw function..." << std::endl;
        png_structp png_ptr_nv = png_ptr;
        png_bytep row_nv = row;
        drawFunction(png_ptr_nv, row_nv, width, height, heightmap);
        std::cout << "  [PNG] Draw function completed!" << std::endl;
    }

    // End write
    std::cout << "  [PNG] Finalizing PNG..." << std::endl;
    png_write_end(png_ptr, nullptr);
    std::cout << "  [PNG] PNG write completed successfully!" << std::endl;

finalise:
    if (fp != nullptr) fclose(fp);
    if (row != nullptr) free(row);
    if (png_ptr != nullptr) {
        if (info_ptr != nullptr) {
            png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
        }
        {
            png_structp png_ptr_nv = png_ptr;
            png_destroy_write_struct(&png_ptr_nv, (png_infopp)nullptr);
        }
    }

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
             static_cast<png_byte>((float)simil_a * ra + (float)simil_b * rb),
             static_cast<png_byte>((float)simil_a * ga + (float)simil_b * gb),
             static_cast<png_byte>((float)simil_a * ba + (float)simil_b * bb));
}

void drawGrayImage(png_structp& png_ptr, png_bytep& row, int width, int height, float *heightmap)
{
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

            setGray(&(row[x*3]), static_cast<int>(res));
        }
        png_write_row(png_ptr, row);
    }
}

void drawColorsImage(png_structp& png_ptr, png_bytep& row, int width, int height, float *heightmap)
{
    std::cout << "  [PNG] drawColorsImage: width=" << width << ", height=" << height << std::endl;
    std::cout << "  [PNG] Computing quantiles..." << std::endl;

    float q15 = find_value_for_quantile(0.15f, heightmap, width * height);
    std::cout << "  [PNG] q15=" << q15 << std::endl;

    float q70 = find_value_for_quantile(0.70f, heightmap, width * height);
    std::cout << "  [PNG] q70=" << q70 << std::endl;

    float q75 = find_value_for_quantile(0.75f, heightmap, width * height);
    std::cout << "  [PNG] q75=" << q75 << std::endl;

    float q90 = find_value_for_quantile(0.90f, heightmap, width * height);
    std::cout << "  [PNG] q90=" << q90 << std::endl;

    float q95 = find_value_for_quantile(0.95f, heightmap, width * height);
    std::cout << "  [PNG] q95=" << q95 << std::endl;

    float q99 = find_value_for_quantile(0.99f, heightmap, width * height);
    std::cout << "  [PNG] q99=" << q99 << std::endl;

    std::cout << "  [PNG] Starting pixel loop..." << std::endl;

    int x, y;
    for (y=0 ; y<height ; y++) {
        // Log every 10 rows to narrow down crash location
        if (y % 10 == 0) {
            std::cout << "  [PNG] Processing row " << y << "/" << height << std::endl;
        }

        // Extra detailed logging for the problematic range
        if (y >= 170 && y <= 180) {
            std::cout << "  [PNG] >> Starting row " << y << " (in crash zone 170-180)" << std::endl;
        }

        for (x=0 ; x<width ; x++) {
            // Validate array access for heightmap
            int index = y*width + x;
            if (index < 0 || index >= width * height) {
                std::cerr << "  [PNG] ERROR: Invalid heightmap index " << index << " at x=" << x << ", y=" << y << std::endl;
                std::cerr << "  [PNG] Array bounds: 0 to " << (width * height - 1) << std::endl;
                throw std::runtime_error("Buffer overflow in drawColorsImage");
            }

            // Validate row buffer access (3 bytes per pixel for RGB)
            int row_index = x * 3;
            size_t row_size = 3 * width;
            if (row_index < 0 || row_index + 2 >= static_cast<int>(row_size)) {
                std::cerr << "  [PNG] ERROR: Invalid row buffer index " << row_index << " at x=" << x << ", y=" << y << std::endl;
                std::cerr << "  [PNG] Row buffer size: " << row_size << " bytes, accessing: " << row_index << " to " << (row_index + 2) << std::endl;
                throw std::runtime_error("Row buffer overflow in drawColorsImage");
            }

            float h = heightmap[index];

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
                continue;
            }

            if (h < q95) {
                gradient(&(row[x*3]), 218, 226, 58, 251, 252, 42, h, q90, q95);
                continue;
            }

            if (h < q99) {
                gradient(&(row[x*3]), 251, 252, 42, 91, 28, 13, h, q95, q99);
                continue;
            }

            gradient(&(row[x*3]), 91, 28, 13, 51, 0, 4, h, q99, 1.0f);
        }

        // Extra logging in crash zone
        if (y >= 170 && y <= 180) {
            std::cout << "  [PNG] >> Completed pixel loop for row " << y << std::endl;
        }

        // Log before writing row to PNG
        if (y % 10 == 0 || (y >= 170 && y <= 180)) {
            std::cout << "  [PNG] About to write row " << y << " to PNG..." << std::endl;
        }

        // Check canary before write
        size_t row_buffer_size = 3 * width * sizeof(png_byte);
        bool canary_ok = true;
        for (int i = 0; i < 16; i++) {
            if (row[row_buffer_size + i] != 0xCD) {
                canary_ok = false;
                break;
            }
        }
        if (!canary_ok) {
            std::cerr << "  [PNG] ERROR: Buffer overrun detected before writing row " << y << "!" << std::endl;
            std::cerr << "  [PNG] Canary bytes at offset " << row_buffer_size << " were corrupted" << std::endl;
            throw std::runtime_error("Buffer overrun in drawColorsImage");
        }

        png_write_row(png_ptr, row);

        if (y % 10 == 0 || (y >= 170 && y <= 180)) {
            std::cout << "  [PNG] Successfully wrote row " << y << std::endl;
        }
    }

    std::cout << "  [PNG] drawColorsImage completed successfully!" << std::endl;
}

int writeImageGray(const char* filename, int width, int height, float *heightmap, const char* title)
{
    return writeImage(filename, width, height, heightmap, title, drawGrayImage);
}

int writeImageColors(const char* filename, int width, int height, float *heightmap, const char* title)
{
    return writeImage(filename, width, height, heightmap, title, drawColorsImage);
}
