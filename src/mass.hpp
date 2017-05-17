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

#ifndef MASS_HPP
#define MASS_HPP

#define NOMINMAX

#include "heightmap.hpp"
#include "dimension.h"

class Mass;

class MassBuilder {
private:
    float mass;           ///< Amount of crust that constitutes the plate.
    ///< X and Y components of the center of mass of plate.
    Platec::vec2f center; 
    
public:
    MassBuilder(const HeightMap& map);
    
    MassBuilder();

    void addPoint(const Platec::vec2ui& point,const float crust);
    Mass build();
};

class IMass {
public:
    virtual float getMass() const = 0;
    virtual const Platec::vec2f  massCenter() const = 0;
};

class Mass : public IMass {
private:
    float mass;           ///< Amount of crust that constitutes the plate.
    ///< X and Y components of the center of mass of plate.
    Platec::vec2f center;     
public:
    Mass(float mass, Platec::vec2f center);
    void incMass(float delta);
    float getMass() const;

    const Platec::vec2f massCenter() const;
    bool isNull() const;
};

#endif
