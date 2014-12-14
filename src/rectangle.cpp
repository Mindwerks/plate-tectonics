#include <stdexcept>
#include "rectangle.hpp"

size_t Rectangle::getMapIndex(size_t* px, size_t* py) const throw()
{
	size_t x = *px % _world_width;
	size_t y = *py % _world_height;

	const size_t ilft = (size_t)(int)_left;
	const size_t itop = (size_t)(int)_top;
	const size_t irgt = (size_t)(int)_right  + (((size_t)(int)_right  < ilft) ? (size_t)(int)_world_width  : 0);
	const size_t ibtm = (size_t)(int)_bottom + (((size_t)(int)_bottom < itop)  ? (size_t)(int)_world_height : 0);
	const int width = irgt - ilft;
	if (width < 0) {
	    throw std::invalid_argument("FAIL");
	}

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

    if (x < 0) {
        throw std::invalid_argument("failure x");
    }
    if (y < 0) {
        throw std::invalid_argument("failure y");
    }

    if (xOk && yOk) {
        *px = x;
        *py = y;
        return (y * width + x);
    } else {
        return -1;
    }
}

void Rectangle::enlarge_to_contain(size_t x, size_t y)
{
    if (y < _top) {
        _top = y;
    }
    if (y > _bottom) {
        _bottom = y;
    }
    if (x < _left) {
        _left = x;
    }
    if (x > _right) {
        _right = x;
    }
}