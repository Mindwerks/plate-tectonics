#include "segment_data.hpp"

SegmentData::SegmentData(Platec::Rectangle& rectangle,
            uint32_t area) : _rectangle(rectangle),
                          _area(area), _coll_count(0) {};

void SegmentData::enlarge_to_contain(uint32_t x, uint32_t y)
{
    _rectangle.enlarge_to_contain(x, y);
};

uint32_t SegmentData::getLeft() const
{
    return _rectangle.getLeft();
};

uint32_t SegmentData::getRight() const
{
    return _rectangle.getRight();
};

uint32_t SegmentData::getTop() const
{
    return _rectangle.getTop();
};

uint32_t SegmentData::getBottom() const
{
    return _rectangle.getBottom();
};

void SegmentData::shift(uint32_t dx, uint32_t dy)
{
    _rectangle.shift(dx, dy);
};

void SegmentData::setLeft(uint32_t v)
{
    _rectangle.setLeft(v);
};

void SegmentData::setRight(uint32_t v)
{
    _rectangle.setRight(v);
};

void SegmentData::setTop(uint32_t v)
{
    _rectangle.setTop(v);
};

void SegmentData::setBottom(uint32_t v)
{
    _rectangle.setBottom(v);
};

bool SegmentData::isEmpty() const
{
    return _area == 0;
};

void SegmentData::incCollCount()
{
    _coll_count++;
};

void SegmentData::incArea()
{
    _area++;
};

void SegmentData::incArea(uint32_t amount)
{
    _area += amount;
};

uint32_t SegmentData::area() const
{
    return _area;
};

uint32_t SegmentData::collCount() const
{
    return _coll_count;
}

void SegmentData::markNonExistent()
{
    _area = 0;
}
