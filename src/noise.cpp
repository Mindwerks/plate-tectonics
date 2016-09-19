/******************************************************************************
 *  plate-tectonics, a plate tectonics simulation library
 *  Copyright (C) 2012-2013 Lauri Viitanen
 *  Copyright (C) 2014-2015 Federico Tomassetti, Bret Curtis
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see http://www.gnu.org/licenses/
 *****************************************************************************/

#include <string>
#include <math.h>
#include "noise.hpp"
#include "sqrdmd.hpp"
#include "simplexnoise.hpp"
#include "utils.hpp"

#if (defined(_MSC_VER) && defined(_M_X64)) || \
    (defined(__APPLE__) && defined(__clang__))
#define sinf(x) static_cast<float>(sin(static_cast<double>(x)))
#define cosf(x) static_cast<float>(cos(static_cast<double>(x)))
#endif

static const float SQRDMD_ROUGHNESS = 0.35f;
static const float SIMPLEX_PERSISTENCE = 0.25f;
#define PI 3.14159265

static uint32_t nearest_pow(uint32_t num)
{
    uint32_t n = 1;

    while (n < num) {
        n <<= 1;
    }

    return n;
}

void createSlowNoise(float* map, const WorldDimension& tmpDim, SimpleRandom randsource)
{
    int64_t seed = randsource.next();
    uint32_t width = tmpDim.getWidth();
    uint32_t height = tmpDim.getHeight();
    float persistence = 0.25f;
    float noiseScale = 0.593;
    float ka = 256/seed;
    float kb = seed*567%256;
    float kc = (seed*seed) % 256;
    float kd = (567-seed) % 256;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float fNX = x/(float)width; // we let the x-offset define the circle
            float fNY = y/(float)height; // we let the x-offset define the circle
            float fRdx = fNX*2*PI; // a full circle is two pi radians
            float fRdy = fNY*4*PI; // a full circle is two pi radians
            float fYSin = sinf(fRdy);
            float fRdsSin = 1.0f;
            float a = fRdsSin*sinf(fRdx);
            float b = fRdsSin*cosf(fRdx);
            float c = fRdsSin*sinf(fRdy);
            float d = fRdsSin*cosf(fRdy);
            float v = scaled_octave_noise_4d(4.0f,
                                             persistence,
                                             0.25f,
                                             0.0f,
                                             1.0f,
                                             ka+a*noiseScale,
                                             kb+b*noiseScale,
                                             kc+c*noiseScale,
                                             kd+d*noiseScale);
            map[y * width + x] = v;
        }
    }
}

void createNoise(float* tmp, const WorldDimension& tmpDim, SimpleRandom randsource, bool useSimplex)
{
    try {
        if (useSimplex) {
            simplexnoise(randsource.next(), tmp,
                         tmpDim.getWidth(),
                         tmpDim.getHeight(),
                         SQRDMD_ROUGHNESS);
        } else {
            uint32_t side = tmpDim.getMax();
            side = nearest_pow(side)+1;
            float* squareTmp = new float[side*side];
            memset(squareTmp, 0, sizeof(float)*side*side);
            for (int y=0; y<tmpDim.getHeight(); y++) {
                memcpy(&squareTmp[y*side],&tmp[y*tmpDim.getWidth()],sizeof(float)*tmpDim.getWidth());
            }
            // to make it tileable we need to insert proper values in the padding area
            // 1) on the right of the valid area
            for (int y=0; y<tmpDim.getHeight(); y++) {
                for (int x=tmpDim.getWidth(); x<side; x++) {
                    // we simply put it as a mix between the east and west border (they should be fairly
                    // similar because it is a toroidal world)
                    squareTmp[y*side+x] = (squareTmp[y*side+0] + squareTmp[y*side+(tmpDim.getWidth()-1)])/2;
                }
            }
            // 2) below the valid area
            for (int y=tmpDim.getHeight(); y<side; y++) {
                for (int x=0; x<side; x++) {
                    // we simply put it as a mix between the north and south border (they should be fairly
                    // similar because it is a toroidal world)
                    squareTmp[y*side+x] = (squareTmp[(0)*side+x] + squareTmp[(tmpDim.getHeight()-1)*side+x])/2;
                }
            }

            sqrdmd(randsource.next(), squareTmp, side, SQRDMD_ROUGHNESS);

            // Calcuate deltas (noise introduced)
            float* deltas = new float[tmpDim.getWidth()*tmpDim.getHeight()];
            for (int y=0; y<tmpDim.getHeight(); y++) {
                for (int x=0; x<tmpDim.getWidth(); x++) {
                    deltas[y*tmpDim.getWidth()+x] = squareTmp[y*side+x]-tmp[y*tmpDim.getWidth()+x];
                }
            }

            // make it tileable
            for (int y=0; y<tmpDim.getHeight(); y++) {
                for (int x=0; x<tmpDim.getWidth(); x++) {
                    int specularX = tmpDim.getWidth() - 1 - x;
                    int specularY = tmpDim.getHeight() -1 - y;
                    float myDelta = deltas[y*tmpDim.getWidth()+x];
                    float specularWidthDelta = deltas[y*tmpDim.getWidth()+specularX];
                    float specularHeightDelta = deltas[specularY*tmpDim.getWidth()+x];
                    float oppositeDelta = deltas[specularY*tmpDim.getWidth()+specularX];
                    tmp[y*tmpDim.getWidth()+x] += (myDelta + specularWidthDelta + specularHeightDelta + oppositeDelta)/4;
                }
            }

            delete[] deltas;
            delete[] squareTmp;
        }
    } catch (const exception& e) {
        std::string msg = "Problem during lithosphere::createNoise, tmpDim+=";
        msg = msg + Platec::to_string(tmpDim.getWidth()) + "x" + Platec::to_string(tmpDim.getHeight()) + " ";
        msg = msg + e.what();
        throw runtime_error(msg.c_str());
    }
}
