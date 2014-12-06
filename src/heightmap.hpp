#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include <stdexcept> // std::invalid_argument

using namespace std;

class HeightMap
{
	public:

	HeightMap(unsigned int width, unsigned int height);
	~HeightMap();
    inline void set(unsigned int x, unsigned y, float value)
    {
        if (x >= _width || y >= _height) {
            throw invalid_argument("invalid coordinates");
        }
        _data[y * _width + x] = value;
    }

    inline float get(unsigned int x, unsigned y)
    {
        if (x >= _width || y >= _height) {
            throw invalid_argument("invalid coordinates");
        }
        return _data[y * _width + x];
    }

	private:

    float* _data;
    unsigned int _width;
    unsigned int _height;
};

#endif
