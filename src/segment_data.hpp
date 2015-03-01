#ifndef SEGMENT_DATA_HPP
#define SEGMENT_DATA_HPP

/// Container for details about a segmented crust area on this plate.
class SegmentData
{
  public:
    SegmentData(Platec::Rectangle& rectangle,
                uint32_t area) : _rectangle(rectangle),
                              _area(_area), _coll_count(0) {};

    void enlarge_to_contain(uint32_t x, uint32_t y)
    {
        _rectangle.enlarge_to_contain(x, y);
    };

    uint32_t getLeft() const
    {
        return _rectangle.getLeft();
    };

    uint32_t getRight() const
    {
        return _rectangle.getRight();
    };

    uint32_t getTop() const
    {
        return _rectangle.getTop();
    };

    uint32_t getBottom() const
    {
        return _rectangle.getBottom();
    };

    void shift(uint32_t dx, uint32_t dy)
    {
        _rectangle.shift(dx, dy);
    };

    void setLeft(uint32_t v)
    {
        _rectangle.setLeft(v);
    };

    void setRight(uint32_t v)
    {
        _rectangle.setRight(v);
    };

    void setTop(uint32_t v)
    {
        _rectangle.setTop(v);
    };

    void setBottom(uint32_t v)
    {
        _rectangle.setBottom(v);
    };

    bool isEmpty() const
    {
        return _area == 0;
    };

    void incCollCount()
    {
        _coll_count++;
    };

    void incArea()
    {
        _area++;
    };

    void incArea(uint32_t amount)
    {
        _area += amount;
    };

    uint32_t area() const
    {
        return _area;
    };

    uint32_t collCount() const
    {
        return _coll_count;
    }

    void markNonExistent()
    {
        _area = 0;
    }

  private:
    Platec::Rectangle _rectangle;
    uint32_t _area; ///< Number of locations this area consists of.
    uint32_t _coll_count; ///< Number of collisions on this segment.
};

#endif
