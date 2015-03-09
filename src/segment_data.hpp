#ifndef SEGMENT_DATA_HPP
#define SEGMENT_DATA_HPP

#include "utils.hpp"
#include "rectangle.hpp"

class ISegmentDataAccess
{
public:    
    virtual uint32_t getLeft() const = 0;
    virtual uint32_t getRight() const = 0;
    virtual uint32_t getTop() const = 0;
    virtual uint32_t getBottom() const = 0;
    virtual bool isEmpty() const = 0;
    virtual uint32_t area() const = 0;
    virtual uint32_t collCount() const = 0;
};

/// Container for details about a segmented crust area on this plate.
class SegmentData : public ISegmentDataAccess
{
  public:
    SegmentData(Platec::Rectangle& rectangle,
                uint32_t area);

    void enlarge_to_contain(uint32_t x, uint32_t y);
    uint32_t getLeft() const;
    uint32_t getRight() const;
    uint32_t getTop() const;
    uint32_t getBottom() const;
    void shift(uint32_t dx, uint32_t dy);
    void setLeft(uint32_t v);
    void setRight(uint32_t v);
    void setTop(uint32_t v);
    void setBottom(uint32_t v);
    bool isEmpty() const;
    void incCollCount();
    void incArea();
    void incArea(uint32_t amount);
    uint32_t area() const;
    uint32_t collCount() const;
    void markNonExistent();
  private:
    Platec::Rectangle _rectangle;
    uint32_t _area; ///< Number of locations this area consists of.
    uint32_t _coll_count; ///< Number of collisions on this segment.
};

#endif
