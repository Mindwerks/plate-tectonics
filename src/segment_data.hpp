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

#ifndef SEGMENT_DATA_HPP
#define SEGMENT_DATA_HPP


#define NOMINMAX

#include "utils.hpp"
#include "vector2D.h"
#include <limits>


class ISegmentData
{
public:
    virtual uint32_t getLeft() const = 0;
    virtual uint32_t getRight() const = 0;
    virtual uint32_t getTop() const = 0;
    virtual uint32_t getBottom() const = 0;
    virtual bool isEmpty() const = 0;
    virtual uint32_t getArea() const = 0;
    virtual uint32_t collCount() const = 0;    
    virtual void incCollCount() = 0;
    virtual void incArea() = 0;
    virtual void enlarge_to_contain(const Platec::vec2ui& point) = 0;
    virtual void markNonExistent() = 0;
    virtual void shift(const Platec::vec2ui& shiftDir) = 0;
    virtual Platec::vec2ui getPointLeftTop() const = 0;
    virtual Platec::vec2ui getPointRightBottom() const = 0;
};

/// Container for details about a segmented crust area on this plate.
class SegmentData : public ISegmentData
{
private:
    Platec::vec2ui pointLeftTop = Platec::vec2ui(0,0);
    Platec::vec2ui pointRightBottom = Platec::vec2ui(0,0);
    uint32_t area; ///< Number of locations this area consists of.
    uint32_t coll_count; ///< Number of collisions on this segment.    
public:
    SegmentData(const Platec::vec2ui& pointLeftTop_,
                const Platec::vec2ui& pointRightBottom_,
                uint32_t area_);

    void enlarge_to_contain(const Platec::vec2ui& point) override;
    uint32_t getLeft() const override;
    uint32_t getRight() const override;
    uint32_t getTop() const override;
    uint32_t getBottom() const override;
    void shift(const Platec::vec2ui& shiftDir) override;
    void setLeft(const uint32_t v);
    void setRight(const uint32_t v);
    void setTop(const uint32_t v);
    void setBottom(const uint32_t v);
    bool isEmpty() const override;
    void incCollCount() override;
    void incArea() override;
    void incArea(const uint32_t amount);
    uint32_t getArea() const override;
    uint32_t collCount() const override;
    void markNonExistent() override;
    Platec::vec2ui getPointLeftTop() const override;
    Platec::vec2ui getPointRightBottom() const override;

};

#endif
