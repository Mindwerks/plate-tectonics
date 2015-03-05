#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include <stdexcept> // std::invalid_argument
#include <cstring>
#include <string>
#include "utils.hpp"
#include "rectangle.hpp"
#include "world_point.hpp"

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

    Matrix(const Matrix<Value>& other)
        : _width(other._width), _height(other._height)
    {
        _data = new Value[_width * _height];
        for (int x=0; x<_width;x++) {
            for (int y=0; y<_height; y++) {
                set(x, y ,other.get(x,y));
            }
        }
    }

    ~Matrix()
    {
        delete[] _data;
    }

    const uint32_t width() const
    {
        return _width;
    }

    const uint32_t height() const
    {
        return _height;
    }    

    const void set_all(const Value& value)
    {
        // we cannot use memset to make it very general
        for (int x=0; x<_width; x++) {
            for (int y=0; y<_height; y++) {
                set(x, y, value);
            }
        }
    }

    inline const Value& set(unsigned int x, unsigned y, const Value& value)
    {
        if (x >= _width || y >= _height) {
            throw invalid_argument("invalid coordinates");
        }
        _data[y * _width + x] = value;
        return value;
    }

    inline const Value& get(unsigned int x, unsigned y) const
    {
        if (x >= _width || y >= _height) {
            throw invalid_argument("invalid coordinates");
        }
        return _data[y * _width + x];
    }

    Matrix<Value>& operator=(const Matrix<Value>& other)
    {
        if (this != &other) // prevent self-assignment
        {
            _width  = other._width;
            _height = other._height;
            delete[] _data;
            _data = new Value[area()];
            for (int x=0; x<_width; x++){
                for (int y=0; y<_height; y++){
                    set(x, y, other.get(x,y));
                }
            }
        }
        return *this;
    }

    Value& operator[](unsigned int index)
    {
        if (index >= area()) {
            string s("invalid index: ");
            s = s + Platec::to_string(index)
                + ", width " + Platec::to_string(_width)
                + ", height " + Platec::to_string(_height);
            throw invalid_argument(s);
        }
        return this->_data[index];
    }

    const Value& operator[](unsigned int index) const
    {
        if (index >= area()) {
            string s("invalid index: ");
            s = s + Platec::to_string(index)
                + ", width " + Platec::to_string(_width)
                + ", height " + Platec::to_string(_height);
            throw invalid_argument(s);
        }
        return this->_data[index];
    }

    Value* raw_data() const
    {
        return _data;
    }

    inline uint32_t area() const
    {
        return _width * _height;
    }   

private:

    Value* _data;
    unsigned int _width;
    unsigned int _height;
};

typedef Matrix<float> HeightMap;
typedef Matrix<uint32_t> AgeMap;

#endif
