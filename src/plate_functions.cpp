#include "plate_functions.hpp"

void calculateCrust(
	uint32_t x, uint32_t y, 
	uint32_t index, 
    float& w_crust, float& e_crust, float& n_crust, float& s_crust,
    uint32_t& w, uint32_t& e, uint32_t& n, uint32_t& s, 
    const WorldDimension& worldDimension,
    HeightMap& map, 
    const uint32_t width,  const uint32_t height)
{
try {    
    // Build masks for accessible directions (4-way).
    // Allow wrapping around map edges if plate has world wide dimensions.
    uint32_t w_mask = -((x > 0)          | (width == worldDimension.getWidth()));
    uint32_t e_mask = -((x < width - 1)  | (width == worldDimension.getWidth()));
    uint32_t n_mask = -((y > 0)          | (height == worldDimension.getHeight()));
    uint32_t s_mask = -((y < height - 1) | (height == worldDimension.getHeight()));

    // Calculate the x and y offset of neighbour directions.
    // If neighbour is out of plate edges, set it to zero. This protects
    // map memory reads from segment faulting.
    w = w_mask==-1 ? worldDimension.xMod(x-1) : 0;
    e = e_mask==-1 ? worldDimension.xMod(x+1) : 0;
    n = n_mask==-1 ? worldDimension.yMod(y-1) : 0;
    s = s_mask==-1 ? worldDimension.yMod(y+1) : 0;

    // Calculate offsets within map memory.
    w = y * width + w;
    e = y * width + e;
    n = n * width + x;
    s = s * width + x;

    // Extract neighbours heights. Apply validity filtering: 0 is invalid.
    w_crust = map[w] * (w_mask & (map[w] < map[index]));
    e_crust = map[e] * (e_mask & (map[e] < map[index]));
    n_crust = map[n] * (n_mask & (map[n] < map[index]));
    s_crust = map[s] * (s_mask & (map[s] < map[index]));    
} catch (const exception& e){
    std::string msg = "Problem during plate::calculateCrust (width: ";
    msg = msg + Platec::to_string(width)
            + ", height: " + Platec::to_string(height) 
            + ", x: " + Platec::to_string(x)
            + ", y:" + Platec::to_string(y) + ") :"
            + e.what();           
    throw runtime_error(msg.c_str());
}
}
