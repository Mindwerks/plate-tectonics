#include <stdexcept>
#include "rectangle.hpp"

namespace Platec {

bool Rectangle::isInside(const uint32_t px, const uint32_t py) const
{
    uint32_t x = px % _worldDimension.getWidth();
    uint32_t y = py % _worldDimension.getHeight();

    const uint32_t ilft = (uint32_t)(int)_left;
    const uint32_t itop = (uint32_t)(int)_top;
    const uint32_t irgt = (uint32_t)(int)_right  + (((uint32_t)(int)_right  < ilft) ? (uint32_t)(int)_worldDimension.getWidth()  : 0);
    const uint32_t ibtm = (uint32_t)(int)_bottom + (((uint32_t)(int)_bottom < itop)  ? (uint32_t)(int)_worldDimension.getHeight() : 0);
    const int width = irgt - ilft;
    if (width < 0) {
        throw std::invalid_argument("FAIL");
    }

    ///////////////////////////////////////////////////////////////////////
    // If you think you're smart enough to optimize this then PREPARE to be
    // smart as HELL to debug it!
    ///////////////////////////////////////////////////////////////////////

    const uint32_t xOkA = (x >= ilft) && (x < irgt);
    const uint32_t xOkB = (x + _worldDimension.getWidth() >= ilft) && (x + _worldDimension.getWidth() < irgt);
    const uint32_t xOk = xOkA || xOkB;

    const uint32_t yOkA = (y >= itop) && (y < ibtm);
    const uint32_t yOkB = (y + _worldDimension.getHeight() >= itop) && (y + _worldDimension.getHeight() < ibtm);
    const uint32_t yOk = yOkA || yOkB;

    x += (x < ilft) ? _worldDimension.getWidth() : 0; // Point is within plate's map: wrap
    y += (y < itop) ? _worldDimension.getHeight() : 0; // it around world edges if necessary.

    x -= ilft; // Calculate offset within local map.
    y -= itop;

    if (x < 0) {
        throw std::invalid_argument("failure x");
    }
    if (y < 0) {
        throw std::invalid_argument("failure y");
    }

    return (xOk && yOk);
}

uint32_t Rectangle::getMapIndex(uint32_t* px, uint32_t* py) const
{
	uint32_t x = *px % _worldDimension.getWidth();
	uint32_t y = *py % _worldDimension.getHeight();

	const uint32_t ilft = (uint32_t)(int)_left;
	const uint32_t itop = (uint32_t)(int)_top;
	const uint32_t irgt = (uint32_t)(int)_right  + (((uint32_t)(int)_right  < ilft) ? (uint32_t)(int)_worldDimension.getWidth()  : 0);
	const uint32_t ibtm = (uint32_t)(int)_bottom + (((uint32_t)(int)_bottom < itop)  ? (uint32_t)(int)_worldDimension.getHeight() : 0);
	const int width = irgt - ilft;
	if (width < 0) {
	    throw std::invalid_argument("FAIL");
	}

	///////////////////////////////////////////////////////////////////////
	// If you think you're smart enough to optimize this then PREPARE to be
	// smart as HELL to debug it!
	///////////////////////////////////////////////////////////////////////

	const uint32_t xOkA = (x >= ilft) && (x < irgt);
	const uint32_t xOkB = (x + _worldDimension.getWidth() >= ilft) && (x + _worldDimension.getWidth() < irgt);
	const uint32_t xOk = xOkA || xOkB;

	const uint32_t yOkA = (y >= itop) && (y < ibtm);
	const uint32_t yOkB = (y + _worldDimension.getHeight() >= itop) && (y + _worldDimension.getHeight() < ibtm);
	const uint32_t yOk = yOkA || yOkB;

	x += (x < ilft) ? _worldDimension.getWidth() : 0; // Point is within plate's map: wrap
	y += (y < itop) ? _worldDimension.getHeight() : 0; // it around world edges if necessary.

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
        throw std::invalid_argument(std::string("Rectangle::mapIndex, index cannot be obtained: x=")
            + Platec::to_string(*px)
            + ", y="+Platec::to_string(*py));
    }
}

void Rectangle::enlarge_to_contain(uint32_t x, uint32_t y)
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

}