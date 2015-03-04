#ifndef SEGMENTS_HPP
#define SEGMENTS_HPP

#include <vector>
#include <cmath>     // sin, cos
#include "simplerandom.hpp"
#include "heightmap.hpp"
#include "rectangle.hpp"
#include "segment_data.hpp"
#include "utils.hpp"
#include "bounds.hpp"
#include "movement.hpp"
#include "mass.hpp"

typedef uint32_t ContinentId;

class Segments
{
public:
	Segments(uint32_t plate_area)
	{
		_area = plate_area;
    	segment = new uint32_t[plate_area];
    	memset(segment, 255, plate_area * sizeof(uint32_t));
	}
	~Segments()
	{	
		delete[] segment;
    	segment = NULL;
    	_area = 0;
	}
	uint32_t area()
	{
		return _area;
	}
	void reset()
	{
		memset(segment, -1, sizeof(uint32_t) * _area);
    	seg_data.clear();
	}
	void reassign(uint32_t newarea, uint32_t* tmps)
	{
		delete[] segment;
        _area = newarea;
        segment = tmps;
	}
	void shift(uint32_t d_lft, uint32_t d_top)
	{
		for (uint32_t s = 0; s < seg_data.size(); ++s)
        {
            seg_data[s].shift(d_lft, d_top);
        }
	}
	uint32_t size() const
	{
		return seg_data.size();
	}
	const SegmentData& operator[](uint32_t index) const
	{
		return seg_data[index];
	}
	SegmentData& operator[](uint32_t index)
	{
		return seg_data[index];
	}
	void add(const SegmentData& data){
		seg_data.push_back(data);
	}
	const ContinentId& id(uint32_t index) const {
		if (index>=_area) {
			throw runtime_error("unvalid index");
		}
		return segment[index];
	}
	ContinentId& id(uint32_t index) {
		if (index>=_area) {
			throw runtime_error("unvalid index");
		}
		return segment[index];
	}
	void setId(uint32_t index, ContinentId id) const {
		if (index>=_area) {
			throw runtime_error("unvalid index");
		}
		/*if (id>=seg_data.size()){
			throw runtime_error("unvalid id");	
		}*/
		segment[index] = id;
	}
private:
	std::vector<SegmentData> seg_data; ///< Details of each crust segment.
	ContinentId* segment;              ///< Segment ID of each piece of continental crust.
	int _area; /// Should be the same as the bounds area of the plate
};

#endif
