#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include <vector>
#include <cmath>     // sin, cos
#include "simplerandom.hpp"
#include "heightmap.hpp"
#include "rectangle.hpp"
#include "segment_data.hpp"
#include "utils.hpp"
#include "bounds.hpp"

#define CONT_BASE 1.0 ///< Height limit that separates seas from dry land.
#define INITIAL_SPEED_X 1
#define DEFORMATION_WEIGHT 2

typedef uint32_t ContinentId;

class plate;

class Movement
{
public:
	Movement(SimpleRandom randsource, const WorldDimension& worldDimension)
		: _randsource(randsource),
		  velocity(1),
		  rot_dir(randsource.next() % 2 ? 1 : -1),
		  dx(0), dy(0),
		  _worldDimension(worldDimension)
	{
		const double angle = 2 * M_PI * _randsource.next_double();
	    vx = cos(angle) * INITIAL_SPEED_X;
	    vy = sin(angle) * INITIAL_SPEED_X;
	}
	void applyFriction(float deformed_mass, float mass)
	{
		float vel_dec = DEFORMATION_WEIGHT * deformed_mass / mass;
        vel_dec = vel_dec < velocity ? vel_dec : velocity;

        // Altering the source variable causes the order of calls to
        // this function to have difference when it shouldn't!
        // However, it's a hack well worth the outcome. :)
        velocity -= vel_dec;
	}
	void move()
	{
		float len;

		// Apply any new impulses to the plate's trajectory.
	    vx += dx;
	    vy += dy;
	    dx = 0;
	    dy = 0;

	    // Force direction of plate to be unit vector.
	    // Update velocity so that the distance of movement doesn't change.
	    len = sqrt(vx*vx+vy*vy);
	    vx /= len;
	    vy /= len;
	    velocity += len - 1.0;
	    velocity *= velocity > 0; // Round negative values to zero.

	    // Apply some circular motion to the plate.
	    // Force the radius of the circle to remain fixed by adjusting
	    // angular velocity (which depends on plate's velocity).
	    uint32_t world_avg_side = (_worldDimension.getWidth() + _worldDimension.getHeight()) / 2;
	    float alpha = rot_dir * velocity / (world_avg_side * 0.33);
	    float _cos = cos(alpha * velocity);
	    float _sin = sin(alpha * velocity);
	    float _vx = vx * _cos - vy * _sin;
	    float _vy = vy * _cos + vx * _sin;
	    vx = _vx;
	    vy = _vy;
	}
	float velocityOnX() const
	{
		return vx * velocity;
	}
	float velocityOnY() const
	{
		return vy * velocity;
	}	
	float velocityOnX(float length) const
	{
		return vx * length;
	}
	float velocityOnY(float length) const
	{
		return vy * length;
	}		
	float dot(float dx_, float dy_) const
	{
		return vx * dx_ + vy * dy_;
	}
	float relativeUnitVelocityOnX(const Movement& m) const;
	float relativeUnitVelocityOnY(const Movement& m) const;
	float momentum(float mass) const throw() { return mass * velocity; }
	float getVelocity() const { return velocity; };
	float velX() const throw() { return vx; }
	float velY() const throw() { return vy; }
	float velocity;       ///< Plate's velocity.
	float rot_dir;        ///< Direction of rotation: 1 = CCW, -1 = ClockWise.
	float dx, dy;         ///< X and Y components of plate's acceleration vector.
	float vx, vy;         ///< X and Y components of plate's direction unit vector.
	void collide(plate& thisPlate, plate& p, Movement& thisMovement, Movement& otherMovement, uint32_t wx, uint32_t wy, float coll_mass);
	void decDx(float delta) { dx -= delta; }
	void decDy(float delta) { dy -= delta; }
private:
	SimpleRandom _randsource;
	const WorldDimension _worldDimension;
};


#endif
