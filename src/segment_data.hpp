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

#include "rectangle.hpp"
#include "utils.hpp"

class ISegmentDataAccess {
  public:
    virtual ~ISegmentDataAccess() = default;
    virtual uint32_t getLeft() const = 0;
    virtual uint32_t getRight() const = 0;
    virtual uint32_t getTop() const = 0;
    virtual uint32_t getBottom() const = 0;
    virtual bool isEmpty() const = 0;
    virtual uint32_t area() const = 0;
    virtual uint32_t collCount() const = 0;
};

class ISegmentData : public ISegmentDataAccess {
  public:
    ~ISegmentData() override = default;
    virtual void incCollCount() = 0;
    virtual void incArea() = 0;
    virtual void enlarge_to_contain(uint32_t x, uint32_t y) = 0;
    virtual void markNonExistent() = 0;
    virtual void shift(uint32_t dx, uint32_t dy) = 0;
};

/// Container for details about a segmented crust area on this plate.
class SegmentData : public ISegmentData {
  public:
    SegmentData(const Platec::Rectangle& rectangle, uint32_t area);

    void enlarge_to_contain(uint32_t x, uint32_t y) override;
    uint32_t getLeft() const override;
    uint32_t getRight() const override;
    uint32_t getTop() const override;
    uint32_t getBottom() const override;
    void shift(uint32_t dx, uint32_t dy) override;
    void setLeft(uint32_t v);
    void setRight(uint32_t v);
    void setTop(uint32_t v);
    void setBottom(uint32_t v);
    bool isEmpty() const override;
    void incCollCount() override;
    void incArea() override;
    void incArea(uint32_t amount);
    uint32_t area() const override;
    uint32_t collCount() const override;
    void markNonExistent() override;

  private:
    Platec::Rectangle _rectangle;
    uint32_t _area;       ///< Number of locations this area consists of.
    uint32_t _coll_count; ///< Number of collisions on this segment.
};

#endif
