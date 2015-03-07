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
	// FIXME: take a HeightMap instead of float*
	MassBuilder(const float* m, const Dimension& dimension);
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
	void incMass(float delta);
	float getMass() const;
	FloatPoint massCenter() const;
	bool null() const;	
private:
	float mass;           ///< Amount of crust that constitutes the plate.
	float cx, cy;         ///< X and Y components of the center of mass of plate.	
	float _totalX, _totalY;	
};

#endif
