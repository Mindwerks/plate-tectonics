#include "mass.hpp"

// ----------------------------------------------
// MassBuilder
// ----------------------------------------------

MassBuilder::MassBuilder(const float* m, const Bounds& bounds)
        : mass(0), cx(0), cy(0)
{
    uint32_t k;
    for (uint32_t y = k = 0; y < bounds.height(); ++y) {
        for (uint32_t x = 0; x < bounds.width(); ++x, ++k) {
            addPoint(x, y, m[k]);
        }
    }
}

MassBuilder::MassBuilder()
        : mass(0), cx(0), cy(0)
{

}

void MassBuilder::addPoint(uint32_t x, uint32_t y, float crust)
{
    if (crust < 0) throw runtime_error("Crust should be not negative");
    mass += crust;
    // Update the center coordinates weighted by mass.
    cx += x * crust;
    cy += y * crust;
}

Mass MassBuilder::build()
{
    if (mass <= 0) {
        return Mass(0, 0, 0);
    } else {
        return Mass(mass, cx/mass, cy/mass);
    }
}

// ----------------------------------------------
// Mass
// ----------------------------------------------

Mass::Mass(float mass_, float cx_, float cy_)
        : mass(mass_), cx(cx_), cy(cy_), _totalX(0), _totalY(0)
{

}

void Mass::incMass(float delta)
{
    mass += delta;
    if (mass < 0) {
        throw runtime_error("(Mass::incMass) does not make sense");
    }
}

float Mass::getMass() const
{
    return mass;
}

float Mass::getCx() const
{
    if (null()) throw runtime_error("(Mass::getCx)");
    return cx;
}

float Mass::getCy() const
{
    if (null()) throw runtime_error("(Mass::getCy)");
    return cx;
}

bool Mass::null() const
{
    return mass <= 0;
}

