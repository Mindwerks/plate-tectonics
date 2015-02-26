#include <string>
#include "noise.hpp"
#include "sqrdmd.hpp"
#include "simplexnoise.hpp"
#include "utils.hpp"

static const float SQRDMD_ROUGHNESS = 0.35f;

static uint32_t nearest_pow(uint32_t num)
{
    uint32_t n = 1;

    while (n < num){
        n <<= 1;
    }

    return n;
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
        for (int y=0; y<tmpDim.getHeight(); y++){
            memcpy(&squareTmp[y*side],&tmp[y*tmpDim.getWidth()],sizeof(float)*tmpDim.getWidth());
        }
        // to make it tileable we need to insert proper values in the padding area
        // 1) on the right of the valid area        
        for (int y=0; y<tmpDim.getHeight(); y++){
        	for (int x=tmpDim.getWidth(); x<side; x++){
        		// we simply put it as a mix between the east and west border (they should be fairly
        		// similar because it is a toroidal world)
        		squareTmp[y*side+x] = (squareTmp[y*side+0] + squareTmp[y*side+(tmpDim.getWidth()-1)])/2;
        	}
        }
        // 2) below the valid area
        for (int y=tmpDim.getHeight(); y<side; y++){
        	for (int x=0; x<side; x++){
        		// we simply put it as a mix between the north and south border (they should be fairly
        		// similar because it is a toroidal world)
        		squareTmp[y*side+x] = (squareTmp[(0)*side+x] + squareTmp[(tmpDim.getHeight()-1)*side+x])/2;
        	}
        }        

        sqrdmd(randsource.next(), squareTmp, side, SQRDMD_ROUGHNESS);

        // Calcuate deltas (noise introduced)
        float* deltas = new float[tmpDim.getWidth()*tmpDim.getHeight()];
        for (int y=0; y<tmpDim.getHeight(); y++){
        	for (int x=0; x<tmpDim.getWidth(); x++){
        		deltas[y*tmpDim.getWidth()+x] = squareTmp[y*side+x]-tmp[y*tmpDim.getWidth()+x];
        	}
        }

        // make it tileable
        for (int y=0; y<tmpDim.getHeight(); y++){
        	for (int x=0; x<tmpDim.getWidth(); x++){
        		int specularX = tmpDim.getWidth() - 1 - x;
        		int specularY = tmpDim.getHeight() -1 - y;
        		float myDelta = deltas[y*tmpDim.getWidth()+x];
        		float specularWidthDelta = deltas[y*tmpDim.getWidth()+specularX];
        		float specularHeightDelta = deltas[specularY*tmpDim.getWidth()+x];
        		float oppositeDelta = deltas[specularY*tmpDim.getWidth()+specularX];
        		tmp[y*tmpDim.getWidth()+x] += (myDelta + specularWidthDelta + specularHeightDelta + oppositeDelta)/4;
        	}
        }
       
        delete[] squareTmp;
    }    
} catch (const exception& e){
    std::string msg = "Problem during lithosphere::createNoise, tmpDim+=";
    msg = msg + Platec::to_string(tmpDim.getWidth()) + "x" + Platec::to_string(tmpDim.getHeight()) + " ";
    msg = msg + e.what();
    throw runtime_error(msg.c_str());
}
}