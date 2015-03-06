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

class Mass;

class MassBuilder
{
public:
	// Deprecated: pass HeightMap instead
	//MassBuilder(const float* m, const Bounds& bounds);
	MassBuilder();
	void addPoint(uint32_t x, uint32_t y, float crust);
	Mass build();
private:
	float mass;           ///< Amount of crust that constitutes the plate.
	float cx, cy;         ///< X and Y components of the center of mass of plate.	
};

class Mass
{
public:
	Mass(float mass_, float cx_, float cy_);
	// Deprecated: pass HeightMap instead
	Mass(const float* m, const Bounds& bounds);
	void incMass(float delta);
	float getMass() const;
	float getCx() const;
	float getCy() const;
	bool null() const;	
private:
	void addPoint(uint32_t x, uint32_t y, float crust);
	void redistribute();
	float mass;           ///< Amount of crust that constitutes the plate.
	float cx, cy;         ///< X and Y components of the center of mass of plate.	
	float _totalX, _totalY;	
};

#endif
