#include "segments.hpp"

Segments::Segments(uint32_t plate_area)
{
    _area = plate_area;
    segment = new uint32_t[plate_area];
    memset(segment, 255, plate_area * sizeof(uint32_t));
}

Segments::~Segments()
{   
    delete[] segment;
    segment = NULL;
    _area = 0;
}

uint32_t Segments::area()
{
    return _area;
}

void Segments::reset()
{
    memset(segment, -1, sizeof(uint32_t) * _area);
    seg_data.clear();
}

void Segments::reassign(uint32_t newarea, uint32_t* tmps)
{
    delete[] segment;
    _area = newarea;
    segment = tmps;
}

void Segments::shift(uint32_t d_lft, uint32_t d_top)
{
    for (uint32_t s = 0; s < seg_data.size(); ++s)
    {
        seg_data[s].shift(d_lft, d_top);
    }
}

uint32_t Segments::size() const
{
    return seg_data.size();
}

const SegmentData& Segments::operator[](uint32_t index) const
{
    return seg_data[index];
}

SegmentData& Segments::operator[](uint32_t index)
{
    return seg_data[index];
}

void Segments::add(const SegmentData& data){
    seg_data.push_back(data);
}

const ContinentId& Segments::id(uint32_t index) const {
    if (index>=_area) {
        throw runtime_error("unvalid index");
    }
    return segment[index];
}

ContinentId& Segments::id(uint32_t index) {
    if (index>=_area) {
        throw runtime_error("unvalid index");
    }
    return segment[index];
}

void Segments::setId(uint32_t index, ContinentId id) const {
    if (index>=_area) {
        throw runtime_error("unvalid index");
    }
    segment[index] = id;
}
