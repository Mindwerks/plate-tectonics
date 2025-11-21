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

#include "rectangle.hpp"
#include "simd_utils.hpp"
#include "utils.hpp"
#include "world_point.hpp"
#include <cstring>
#include <stdexcept> // std::invalid_argument
#include <string>

using namespace std;

template <typename Value>
class Matrix {
  public:
    Matrix(unsigned int width, unsigned int height) : m_width(width), m_height(height) {
        ASSERT(width != 0 && height != 0, "Matrix width and height should be greater than zero");
        m_area = width * height;
        m_data = new Value[m_area];
    }
    Matrix(Value* data, unsigned int width, unsigned int height)
        : m_width(width), m_height(height) {
        ASSERT(data != 0 && width != 0 && height != 0, "Invalid matrix data");
        m_area = width * height;
        m_data = data;
    }

    Matrix(const Matrix<Value>& other)
        : m_width(other.m_width), m_height(other.m_height), m_area(other.m_area) {
        m_data = new Value[m_area];
        copy(other);
    }

    Matrix(Matrix<Value>&& other) noexcept
        : m_data(other.m_data), m_width(other.m_width), m_height(other.m_height),
          m_area(other.m_area) {
        other.m_data = nullptr;
        other.m_width = 0;
        other.m_height = 0;
        other.m_area = 0;
    }

    ~Matrix() {
        delete[] m_data;
    }

    void set_all(const Value& value) {
        // Use SIMD-optimized implementation for float types
        // Falls back to scalar loop for other types
        const uint32_t MY_AREA = area();
        if (sizeof(Value) == sizeof(float)) {
            simd::set_all(reinterpret_cast<float*>(m_data), MY_AREA,
                          *reinterpret_cast<const float*>(&value));
        } else {
            // Generic fallback for non-float types
            for (uint32_t i = 0; i < MY_AREA; i++) {
                m_data[i] = value;
            }
        }
    }
    void copy(const Matrix& other) {
        if (m_area != other.m_area) {
            m_width = other.m_width;
            m_height = other.m_height;
            m_area = other.m_area;
            delete[] m_data;
            m_data = new Value[m_area];
        }
        // Use fast memcpy for contiguous data
        memcpy(m_data, other.m_data, m_area * sizeof(Value));
    }

    const Value& set(unsigned int x, unsigned y, const Value& value) {
        ASSERT(x < m_width && y < m_height, "Invalid coordinates");
        m_data[(y * m_width) + x] = value;
        return value;
    }

    const Value& get(unsigned int x, unsigned y) const {
        ASSERT(x < m_width && y < m_height, "Invalid coordinates");
        return m_data[(y * m_width) + x];
    }

    Matrix<Value>& operator=(const Matrix<Value>& other) {
        if (this != &other) {
            copy(other);
        }
        return *this;
    }

    Matrix<Value>& operator=(Matrix<Value>&& other) noexcept {
        if (this != &other) {
            delete[] m_data;
            m_data = other.m_data;
            m_width = other.m_width;
            m_height = other.m_height;
            m_area = other.m_area;
            other.m_data = nullptr;
            other.m_width = 0;
            other.m_height = 0;
            other.m_area = 0;
        }
        return *this;
    }

    Value& operator[](unsigned int index) {
        return this->m_data[index];
    }

    const Value& operator[](unsigned int index) const {
        return this->m_data[index];
    }

    Value* raw_data() const {
        return m_data;
    }
    [[nodiscard]] uint32_t width() const {
        return m_width;
    }
    [[nodiscard]] uint32_t height() const {
        return m_height;
    }
    [[nodiscard]] uint32_t area() const {
        return m_area;
    }

  private:
    Value* m_data;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_area;
};

using HeightMap = Matrix<float>;
using AgeMap = Matrix<uint32_t>;
using IndexMap = Matrix<uint32_t>;

#endif
