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

#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include <stdexcept> // std::invalid_argument
#include <cstring>
#include <string>
#include "utils.hpp"
#include "rectangle.hpp"
#include "world_point.hpp"
#include "simd_utils.hpp"

using namespace std;

template <typename Value>
class Matrix
{
public:

    Matrix(unsigned int width, unsigned int height)
        : _width(width), _height(height)
    {
        ASSERT(width != 0 && height != 0, "Matrix width and height should be greater than zero");
        _area = width * height;
        _data = new Value[_area];
    }
    Matrix(Value* data, unsigned int width, unsigned int height)
        : _width(width), _height(height) {
        ASSERT(data != 0 && width != 0 && height != 0, "Invalid matrix data");
        _area = width * height;
        _data = data;
    }

    Matrix(const Matrix<Value>& other)
        : _width(other._width), _height(other._height), _area(other._area)
    {
        _data = new Value[_area];
        copy(other);
    }

    ~Matrix()
    {
        delete[] _data;
    }

    void set_all(const Value& value)
    {
        // Use SIMD-optimized implementation for float types
        // Falls back to scalar loop for other types
        const uint32_t my_area = area();
        if (sizeof(Value) == sizeof(float)) {
            simd::set_all(reinterpret_cast<float*>(_data), my_area,
                          *reinterpret_cast<const float*>(&value));
        } else {
            // Generic fallback for non-float types
            for (uint32_t i = 0; i < my_area; i++) {
                _data[i] = value;
            }
        }
    }
    void copy(const Matrix& other)
    {
        if (_area != other._area) {
            _width = other._width;
            _height = other._height;
            _area = other._area;
            delete[] _data;
            _data = new Value[_area];
        }
        // Use fast memcpy for contiguous data
        memcpy(_data, other._data, _area * sizeof(Value));
    }

    inline const Value& set(unsigned int x, unsigned y, const Value& value)
    {
        ASSERT(x < _width && y < _height, "Invalid coordinates");
        _data[y * _width + x] = value;
        return value;
    }

    inline const Value& get(unsigned int x, unsigned y) const
    {
        ASSERT(x < _width && y < _height, "Invalid coordinates");
        return _data[y * _width + x];
    }

    Matrix<Value>& operator=(const Matrix<Value>& other)
    {
        copy(other);
        return *this;
    }

    Value& operator[](unsigned int index)
    {
        return this->_data[index];
    }

    const Value& operator[](unsigned int index) const
    {
        return this->_data[index];
    }

    Value* raw_data() const
    {
        return _data;
    }
    const uint32_t width() const
    {
        return _width;
    }
    const uint32_t height() const
    {
        return _height;
    }
    inline uint32_t area() const
    {
        return _area;
    }
private:

    Value* _data;
    unsigned int _width;
    unsigned int _height;
    unsigned int _area;
};

typedef Matrix<float> HeightMap;
typedef Matrix<uint16_t> AgeMap;      // uint16_t: 65K step limit, saves 50% memory
typedef Matrix<uint8_t> IndexMap;     // uint8_t: 255 max plates, saves 75% memory

#endif
