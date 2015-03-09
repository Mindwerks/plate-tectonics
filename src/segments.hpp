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

class ISegmentCreator
{
public:
	virtual ContinentId createSegment(uint32_t wx, uint32_t wy) const = 0;
};

class Segments
{
public:
	Segments(uint32_t plate_area);
	~Segments();
	void setSegmentCreator(ISegmentCreator* segmentCreator)
	{
		_segmentCreator = segmentCreator;
	}
	void setBounds(Bounds* bounds)
	{
		_bounds = bounds;
	}
	uint32_t area();
	void reset();
	void reassign(uint32_t newarea, uint32_t* tmps);
	void shift(uint32_t d_lft, uint32_t d_top);
	uint32_t size() const;
	const SegmentData& operator[](uint32_t index) const;
	SegmentData& operator[](uint32_t index);
	void add(const SegmentData& data);
	const ContinentId& id(uint32_t index) const;
	ContinentId& id(uint32_t index);
	void setId(uint32_t index, ContinentId id) const;
	ContinentId getContinentAt(int x, int y) const;
private:
	std::vector<SegmentData> seg_data; ///< Details of each crust segment.
	ContinentId* segment;              ///< Segment ID of each piece of continental crust.
	int _area; /// Should be the same as the bounds area of the plate
	ISegmentCreator* _segmentCreator;
	Bounds* _bounds;
};

#endif
