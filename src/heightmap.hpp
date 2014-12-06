#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include <stdexcept> // std::invalid_argument

using namespace std;

template <typename Value>
class Matrix
{
	public:

    Matrix(unsigned int width, unsigned int height)
        : _width(width), _height(height)
    {
        if (width == 0 || height == 0) {
            throw invalid_argument("width and height should be greater than zero");
        }
        _data = new Value[width * height];
    };

    ~Matrix()
    {
        delete _data;
    }

    inline void set(unsigned int x, unsigned y, Value value)
    {
        if (x >= _width || y >= _height) {
            throw invalid_argument("invalid coordinates");
        }
        _data[y * _width + x] = value;
    }

    inline Value get(unsigned int x, unsigned y)
    {
        if (x >= _width || y >= _height) {
            throw invalid_argument("invalid coordinates");
        }
        return _data[y * _width + x];
    }

	private:

    Value* _data;
    unsigned int _width;
    unsigned int _height;
};

typedef Matrix<float> HeightMap;

#endif
