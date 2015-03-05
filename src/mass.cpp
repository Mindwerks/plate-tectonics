#include "mass.hpp"

Mass::Mass(const float* m, const Bounds& bounds)
		: mass(0), 
          cx(0), cy(0)
{
    uint32_t k;
    for (uint32_t y = k = 0; y < bounds.height(); ++y) {
        for (uint32_t x = 0; x < bounds.width(); ++x, ++k) {
            // Clone map data and count crust mass.
            mass += m[k];

            // Calculate center coordinates weighted by mass.
            cx += x * m[k];
            cy += y * m[k];
        }
    }

    // Normalize center of mass coordinates.
    cx /= mass;
    cy /= mass;
}

void Mass::incMass(float delta)
{
	mass += delta;
}

float Mass::getMass() const
{
	return mass;
}

float Mass::getCx() const
{
	return cx;
}

float Mass::getCy() const
{
	return cx;
}

bool Mass::notNull() const
{
	return mass > 0;
}

void Mass::reset()
{
	mass = 0;
		cx = cy = 0;
}

void Mass::addPoint(uint32_t x, uint32_t y, float crust)
{
	if (crust < 0) throw runtime_error("Crust should be not negative");
	mass += crust;
    // Update the center coordinates weighted by mass.
    cx += x * crust;
    cy += y * crust;
}

void Mass::redistribute()
{
	if (mass > 0) {
    	cx /= mass;
    	cy /= mass;
  	}
}
