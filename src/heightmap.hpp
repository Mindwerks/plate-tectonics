#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include <stdexcept> // std::invalid_argument
#include <cstring>
#include <string>
#include "utils.hpp"

using namespace std;

class Position
{
public:
    Position(unsigned int x, unsigned int y);
    Position(const Position& other);
    unsigned int x() const;
    unsigned int y() const;
private:
    unsigned int _x;
    unsigned int _y;
};

template <typename Value>
class Matrix
{
public:

    Matrix(unsigned int width, unsigned int height)
        : _width(width), 
          _height(height)
    {
        if (width == 0 || height == 0) {
            throw invalid_argument("width and height should be greater than zero");
        }
        _data = new Value[width * height];
    };

    Matrix( const Matrix<Value>& other )
        : _width(other._width), 
          _height(other._height)
    {
        _data = new Value[_width * _height];
        for (int x=0; x<_width;x++){
            for (int y=0; y<_height;y++){
                set(x,y,other.get(x,y));
            }
        }
    }

    ~Matrix()
    {
        delete[] _data;
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

    inline Value get(unsigned int x, unsigned y) const
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
            _data = new Value[_width * _height];
            for (int x=0; x<_width; x++){
                for (int y=0; y<_height; y++){
                    set(x, y, other.get(x,y));
                }
            }
        }
        return *this;
    }

    Value& operator[](unsigned int index) const
    {
        if (index >= (_width*_height)) {
            string s("invalid index: ");
            s = s + Platec::to_string(index)s
                + ", width " + Platec::to_string(_width)
                + ", height " + Platec::to_string(_height);
            throw invalid_argument(s);
        }
        return this->_data[index];
    }

    Value& operator[](const Position& pos) const
    {
        if (pos.x() >= _width || pos.y() >= _height) {
            string s("invalid index: ");
            s = s
                + Platec::to_string(pos.x())
                + ", "
                + Platec::to_string(pos.y())
                + ", width " + Platec::to_string(_width)
                + ", height " + Platec::to_string(_height);
            throw invalid_argument(s);
        }
        return this->_data[pos.y() * _width + pos.y()];
    }

    void copy_raw_to(Value*& dst, bool allocate = false) const
    {
        if (allocate) {
            dst = new Value[_width * _height];
        }
        memcpy(dst, _data, sizeof(Value) * _width * _height);
    }

    void from(Value* src)
    {
        memcpy(_data, src, sizeof(Value) * _width * _height);
    }

    bool equals(Value* other)
    {                
        for (int i=0; i< (_width *_height); i++){
            if (_data[i] != other[i]){
                return false;
            }
        }
        return true;
    }

    Value* raw_data() const
    {
        return _data;
    }

    uint32_t area() const
    {
        return _width * _height;
    }

    int xMod(int x) const
    {
        while (x < 0) {
            x += _width;
        }
        return x % _width;
    }

    int yMod(int y) const
    {
        while (y < 0) {
            y += _height;
        }
        return y % _height;
    }    

private:

    Value* _data;
    unsigned int _width;
    unsigned int _height;
};

typedef Matrix<float>  HeightMap;
typedef Matrix<uint32_t> AgeMap;

#endif
