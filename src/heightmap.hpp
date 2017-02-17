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


#include <stdint.h>
#include <vector>


template <typename Value>
class Matrix
{
private:

    std::vector<Value> data;
    uint32_t dataWidth;
    uint32_t dataHeight;    
    
    uint32_t calcIndex(const uint32_t x,const uint32_t y) const
    {
        return y * dataWidth + x;
    }
    
public:
    
    Matrix(uint32_t width, uint32_t height)
        : dataWidth(width), dataHeight(height), data(width*height)
    {
    }
    Matrix(Value* data, uint32_t width, uint32_t height)
        : dataWidth(width), dataHeight(height), data(data, data+(width*height))
    {
    }

    void set_all(const Value& value) 
    {
        data = std::vector<Value>(data.size(),value);
    }
    

    const Value& set(const uint32_t x,const uint32_t y, const Value& value)
    {
        data.at(calcIndex(x,y)) = value;
        return value;
    }

    const Value& get(const uint32_t x,const uint32_t y) const
    {
        return data.at(calcIndex(x,y));
    }
    
    const Value& get(uint32_t index) const
    {
        return data.at(index);
    }
    
    Value& operator[](uint32_t index)
    {
        return data.at(index);
    }

    const Value& operator[](uint32_t index) const
    {
        return data.at(index);;
    }

    const Value* raw_data() const
    {
        //cant use .data() because msvc
        return &data[0];
    }
    
    Value* raw_data()
    {
        //cant use .data() because msvc
        return &data[0];
    }
        
    const uint32_t width() const
    {
        return dataWidth;
    }
    const uint32_t height() const
    {
        return dataHeight;
    }
    uint32_t area() const
    {
        return data.size();
    }
    



};

typedef Matrix<float> HeightMap;
typedef Matrix<uint32_t> AgeMap;
typedef Matrix<uint32_t> IndexMap;

#endif
