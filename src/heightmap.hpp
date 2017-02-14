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
#include <vector>
#include "utils.hpp"
#include "rectangle.hpp"
#include "world_point.hpp"

template <typename Value>
class Matrix
{
private:

    std::vector<Value> data;
    uint32_t dataWidth;
    uint32_t dataHeight;    
    
public:

    Matrix(unsigned int width, unsigned int height)
        : dataWidth(width), dataHeight(height), data(width*height)
    {
    }
    Matrix(Value* data, unsigned int width, unsigned int height)
        : dataWidth(width), dataHeight(height), data(data, data+(width*height))
    {
    }

    void set_all(const Value& value) 
    {
        data = std::vector<Value>(data.size(),value);
    }
    
    void copy(const Matrix& other)
    {
        if (data.size() != other.data.size()) 
        {
            dataWidth = other.dataWidth;
            dataHeight = other.dataHeight;
            data = other.data;
        }
    }

    const Value& set(unsigned int x, unsigned y, const Value& value)
    {
        data.at(y * dataWidth + x) = value;
        return value;
    }

    const Value& get(unsigned int x, unsigned y) const
    {
        return data.at(y * dataWidth + x);
    }

    Value& operator[](unsigned int index)
    {
        return data[index];
    }

    const Value& operator[](unsigned int index) const
    {
        return data[index];
    }

    const Value* raw_data() const
    {
        return data.data();
    }
    
    Value* raw_data()
    {
        return data.data();
    }
    

    
    const uint32_t width() const
    {
        return dataWidth;
    }
    const uint32_t height() const
    {
        return dataHeight;
    }
    inline uint32_t area() const
    {
        return data.size();
    }

};

typedef Matrix<float> HeightMap;
typedef Matrix<uint32_t> AgeMap;
typedef Matrix<uint32_t> IndexMap;

#endif
