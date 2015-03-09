#ifndef BOUNDS_HPP
#define BOUNDS_HPP

#include "simplerandom.hpp"
#include "heightmap.hpp"
#include "rectangle.hpp"
#include "segment_data.hpp"
#include "utils.hpp"

#include <stdio.h>

class IBounds {
public:

	/// Accept plate relative coordinates and return the index inside the plate.
	/// The index can be used with other classes to retrieve information about specific points.
	virtual uint32_t index(uint32_t x, uint32_t y) const = 0;
	virtual uint32_t area() const = 0;
	virtual uint32_t width() const = 0;
	virtual uint32_t height() const = 0;
	virtual uint32_t leftAsUint() const = 0;
	virtual uint32_t topAsUint() const = 0;
	virtual uint32_t rightAsUintNonInclusive() const = 0;
	virtual uint32_t bottomAsUintNonInclusive() const = 0;
	/// Given a point in World relative coordinates, it tells if it is part of the plate or not.
	virtual bool containsWorldPoint(uint32_t x, uint32_t y) const = 0;
	/// Given a point in plate relative coordinates, it tells if it is part of the plate or not.
	virtual bool isInLimits(float x, float y) const = 0;
	/// Shift the position of the top left corner by the given amount.
	/// It preserves the dimension of the plate.
	virtual void shift(float dx, float dy) = 0;
	virtual void grow(int dx, int dy) = 0;
	/// Translate world coordinates into offset within plate's height map.
	///
	/// If the global world map coordinates are within plate's height map,
	/// the values of passed coordinates will be altered to contain the
	/// X and y offset within the plate's height map. Otherwise an exception is thrown.
	///
	/// @param[in, out] x	Offset on the global world map along X axis.
	/// @param[in, out] y	Offset on the global world map along Y axis.
	/// @return		        Offset in height map or BAD_INDEX on error.
	virtual uint32_t getValidMapIndex(uint32_t* px, uint32_t* py) const = 0;
	/// Translate world coordinates into offset within plate's height map.
	///
	/// Iff the global world map coordinates are within plate's height map,
	/// the values of passed coordinates will be altered to contain the
	/// X and y offset within the plate's height map. Otherwise values are
	/// left intact.
	///
	/// @param[in, out] x	Offset on the global world map along X axis.
	/// @param[in, out] y	Offset on the global world map along Y axis.
	/// @return		        Offset in height map
	virtual uint32_t getMapIndex(uint32_t* x, uint32_t* y) const = 0;
};

/// Plate bounds.
class Bounds : public IBounds
{
public:

	/// @param worldDimension dimension of the world containing the plate
	/// @param position Position of the top left corner of the plae
	/// @param dimension Dimension of the plate
	Bounds(const WorldDimension& worldDimension, 
		   const FloatPoint& position, 
		   const Dimension& dimension);

	/// Accept plate relative coordinates and return the index inside the plate.
	/// The index can be used with other classes to retrieve information about specific points.
	uint32_t index(uint32_t x, uint32_t y) const;

	uint32_t area() const;

	uint32_t width() const;

	uint32_t height() const;

	uint32_t leftAsUint() const;

	uint32_t topAsUint() const;

	uint32_t rightAsUintNonInclusive() const;

	uint32_t bottomAsUintNonInclusive() const;

	/// Given a point in World relative coordinates, it tells if it is part of the plate or not.
	bool containsWorldPoint(uint32_t x, uint32_t y) const;

	/// Given a point in plate relative coordinates, it tells if it is part of the plate or not.
	bool isInLimits(float x, float y) const;

	/// Shift the position of the top left corner by the given amount.
	/// It preserves the dimension of the plate.
	void shift(float dx, float dy);

	void grow(int dx, int dy);

	/// Translate world coordinates into offset within plate's height map.
	///
	/// If the global world map coordinates are within plate's height map,
	/// the values of passed coordinates will be altered to contain the
	/// X and y offset within the plate's height map. Otherwise an exception is thrown.
	///
	/// @param[in, out] x	Offset on the global world map along X axis.
	/// @param[in, out] y	Offset on the global world map along Y axis.
	/// @return		        Offset in height map or BAD_INDEX on error.
	uint32_t getValidMapIndex(uint32_t* px, uint32_t* py) const;

	/// Translate world coordinates into offset within plate's height map.
	///
	/// Iff the global world map coordinates are within plate's height map,
	/// the values of passed coordinates will be altered to contain the
	/// X and y offset within the plate's height map. Otherwise values are
	/// left intact.
	///
	/// @param[in, out] x	Offset on the global world map along X axis.
	/// @param[in, out] y	Offset on the global world map along Y axis.
	/// @return		        Offset in height map
	uint32_t getMapIndex(uint32_t* x, uint32_t* y) const;
	
private:

	/// Return a rectangle representing the Bounds inside the world.
	Platec::Rectangle asRect() const;

	const WorldDimension _worldDimension;
	FloatPoint _position;
	Dimension _dimension;	
};

#endif
