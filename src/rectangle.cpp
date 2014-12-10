#include "rectangle.hpp"

size_t Rectangle::getMapIndex(size_t* px, size_t* py) const throw()
{
	size_t x = *px % _world_width;
	size_t y = *py % _world_height;

	const size_t ilft = (size_t)(int)_left;
	const size_t itop = (size_t)(int)_top;
	const size_t irgt = _right;
	const size_t ibtm = _bottom;
	const size_t width = _right - _left;

	///////////////////////////////////////////////////////////////////////
	// If you think you're smart enough to optimize this then PREPARE to be
	// smart as HELL to debug it!
	///////////////////////////////////////////////////////////////////////

	const size_t xOkA = (x >= ilft) && (x < irgt);
	const size_t xOkB = (x + _world_width >= ilft) && (x + _world_width < irgt);
	const size_t xOk = xOkA || xOkB;

	const size_t yOkA = (y >= itop) && (y < ibtm);
	const size_t yOkB = (y + _world_height >= itop) && (y + _world_height < ibtm);
	const size_t yOk = yOkA || yOkB;

	x += (x < ilft) ? _world_width : 0; // Point is within plate's map: wrap
	y += (y < itop) ? _world_height : 0; // it around world edges if necessary.

	x -= ilft; // Calculate offset within local map.
	y -= itop;

    if (xOk && yOk) {
        *px = x;
        *py = y;
        return (y * width + x);
    }
    else {
        return -1;
    }
}