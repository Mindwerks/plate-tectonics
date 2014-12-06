#include "heightmap.hpp"

#include <stdexcept> // std::invalid_argument

using namespace std;

HeightMap::HeightMap(unsigned int width, unsigned int height)
    : _width(width), _height(height)
{
    if (width == 0 || height == 0) {
        throw invalid_argument("width and height should be greater than zero");
    }
    _data = new float[width * height];
}

HeightMap::~HeightMap()
{
    delete _data;
}

