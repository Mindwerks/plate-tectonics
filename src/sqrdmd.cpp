/******************************************************************************
 *  plate-tectonics, a plate tectonics simulation library
 *  Copyright (C) 2012-2013 Lauri Viitanen
 *  Copyright (C) 2014-2015 Federico Tomassetti, Bret Curtis
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see http://www.gnu.org/licenses/
 *****************************************************************************/

/** @file	hmapgen_sqrdmd.c
 *  @brief	Contains functions to generate fractal height maps.
 *
 *  @author Lauri Viitanen
 *  @date 2011-08-09
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#ifdef __MINGW32__ // this is to avoid a problem with the hypot function which is messed up by Python...
#undef __STRICT_ANSI__
#endif
#include "simplerandom.hpp"

#include "sqrdmd.hpp"

using namespace std;

#define CALC_SUM(a, b, c, d, rnd)\
{\
	sum = ((a) + (b) + (c) + (d)) * 0.25f;\
	sum = sum + slope * rnd;\
}

#define SAVE_SUM(a)\
{\
	bool isZero = (int)map[a] == 0;  \
	if (isZero) { \
		map[a] = sum; \
	} \
}

void normalize(float* arr, int size)
{
	float min = arr[0], max = arr[0], diff;

	for (int i = 1; i < size; ++i)
	{
		min = min < arr[i] ? min : arr[i];
		max = max > arr[i] ? max : arr[i];
	}

	diff = max - min;

	if (diff > 0)
		for (int i = 0; i < size; ++i)
			arr[i] = (arr[i] - min) / diff;
}

class Coord 
{
public:
	Coord(int width, int height) : _width(width), _height(height)
	{};
	int indexOf(int x, int y) const
	{
		ASSERT(x >= 0 && x < _width && y >= 0 && y < _height,
			   "Coordinates are not valid");
		return y * _width + x;
	}
private:
	int _width, _height;
};

int sqrdmd(long seed, float* map, int size, float rgh)
{
	SimpleRandom _randsource(seed);
	
	const int full_size = size * size;
	
	int i, temp;
	int x, y, dx, dy;
	int x0, x1, y0, y1;
	int p0, p1, p2, p3;
	int step, line_jump, masked;
	float slope, sum, center_sum;
	i = 0;
	temp = size - 1;
	// MUST EQUAL TO 2^x + 1!
	ASSERT(!(temp & (temp - 1) || temp & 3), "Side should be 2**n +1");
	temp = size;
	slope = rgh;
	step = size & ~1;
	
	/* Calculate midpoint ("diamond step"). */
	dy = step * size;
	CALC_SUM(map[0], map[step], map[dy], map[dy + step], _randsource.next_float_signed());
	SAVE_SUM(i);
	center_sum = sum;
	
	/* Calculate each sub diamonds' center points ("square step"). */
	/* Top row. */
	p0 = step >> 1;
	CALC_SUM(map[0], map[step], center_sum, center_sum, _randsource.next_float_signed());
	SAVE_SUM(p0);
	/* Left column. */
	p1 = p0 * size;
	CALC_SUM(map[0], map[dy], center_sum, center_sum, _randsource.next_float_signed());
	SAVE_SUM(p1);
	map[full_size + p0 - size] = map[p0]; /* Copy top val into btm row. */
	map[p1 + size - 1] = map[p1]; /* Copy left value into right column. */
	slope *= rgh;
	step >>= 1;
	
	while (step > 1) /* Enter the main loop. */
	{
		/*************************************************************
		* Calc midpoint of sub squares on the map ("diamond step"). *
		*************************************************************/
		dx = step;
		dy = step * size;
		i = (step >> 1) * (size + 1);
		line_jump = step * size + 1 + step - size;
		for (y0 = 0, y1 = dy; y1 < size * size; y0 += dy, y1 += dy)
		{
			for (x0 = 0, x1 = dx; x1 < size; x0 += dx, x1 += dx, i += step)
			{
				sum = (map[y0+x0] + map[y0+x1] +
				map[y1+x0] + map[y1+x1]) * 0.25f;
				sum = sum + slope * _randsource.next_float_signed();
				masked = !((int)map[i]);
				map[i] = map[i] * !masked + sum * masked;
			}
			/* There's additional step taken at the end of last
			* valid loop. That step actually isn't valid because
			* the row ends right then. Thus we are forced to
			* manually remove it after the loop so that 'i'
			* points again to the index accessed last.
			*/
			i += line_jump - step;
		}
	
		/**************************************************************
		* Calculate each sub diamonds' center point ("square step").
		* Diamond gets its left and right vertices from the square
		* corners of last iteration and its top and bottom vertices
		* from the "diamond step" we just performed.
		*************************************************************/
		i = step >> 1;
		p0 = step; /* right */
		p1 = i * size + i; /* bottom */
		p2 = 0; /* left */
		p3 = full_size + i - (i + 1) * size; /* top (wrapping edges) */
	
		/* Calculate "diamond" values for top row in map. */
		while (p0 < size)
		{
			sum = (map[p0] + map[p1] + map[p2] + map[p3]) * 0.25f;
			sum = sum + slope * _randsource.next_float_signed();
			masked = !((int)map[i]);
			map[i] = map[i] * !masked + sum * masked;
			/* Copy it into bottom row. */
			map[full_size + i - size] = map[i];
			p0 += step; p1 += step; p2 += step;
			p3 += step; i += step;
		}

		/* Now that top row's values are calculated starting from
		* 'y = step >> 1' both saves us from recalculating same things
		* twice and guarantees that data will not be read beyond top
		* row of map. 'size - (step >> 1)' guarantees that data will
		* not be read beyond bottom row of map.
		*/
		for (y = step >> 1, temp = 0; y < size - (step >> 1); y += step >> 1, temp = !temp)
		{
			p0 = step >> 1; /* right */
			p1 = p0 * size; /* bottom */
			p2 = -p0; /* left */
			p3 = -p1; /* top */
			/* For even rows add step/2. Otherwise add nothing. */
			x = i = p0 * temp; /* Init 'x' while it's easy. */
			i += y * size; /* Move 'i' into correct row. */
			p0 += i;
			p1 += i;
			/* For odd rows p2 (left) wraps around map edges. */
			p2 += i + (size - 1) * !temp;
			p3 += i;
			/* size - (step >> 1) guarantees that data will not be
			* read beyond rightmost column of map. */
			for (; x < size - (step >> 1); x += step)
			{
				sum = (map[p0] + map[p1] +
				map[p2] + map[p3]) * 0.25f;
				sum = sum + slope * _randsource.next_float_signed();
				masked = !((int)map[i]);
				map[i] = map[i] * !masked + sum * masked;
				p0 += step;
				p1 += step;
				p2 += step;
				p3 += step;
				i += step;
				/* if we start from leftmost column -> left
				* point (p2) is going over the right border ->
				* wrap it around into the beginning of
				* previous rows left line. */
				p2 -= (size - 1) * !x;
			}
			/* copy rows first element into its last */
			i = y * size;
			map[i + size - 1] = map[i];
		}
		slope *= rgh; /* reduce amount of randomness for next round */
		step >>= 1; /* split squares and diamonds in half */
	}
	return (0);
}
