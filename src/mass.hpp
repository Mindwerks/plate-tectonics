#ifndef MASS_HPP
#define MASS_HPP

#include <vector>
#include <cmath>     // sin, cos
#include "simplerandom.hpp"
#include "heightmap.hpp"
#include "rectangle.hpp"
#include "segment_data.hpp"
#include "utils.hpp"
#include "bounds.hpp"
#include "movement.hpp"

class Mass
{
public:
	Mass(const float* m, const Bounds& bounds)
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
	void incMass(float delta)
	{
		mass += delta;
	}
	float getMass()
	{
		return mass;
	}
	bool notNull()
	{
		return mass > 0;
	}
	void reset()
	{
		mass = 0;
  		cx = cy = 0;
	}
	void addPoint(uint32_t x, uint32_t y, float crust)
	{
		if (crust < 0) throw runtime_error("Crust should be not negative");
		mass += crust;
	    // Update the center coordinates weighted by mass.
	    cx += x * crust;
	    cy += y * crust;
	}
	void redistribute()
	{
		if (mass > 0) {
	    	cx /= mass;
	    	cy /= mass;
	  	}
	}
	float mass;           ///< Amount of crust that constitutes the plate.
	float cx, cy;         ///< X and Y components of the center of mass of plate.	
private:
};

#endif
