/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Dimension.h
 * Author: ro-lip
 *
 * Created on 16. Mai 2017, 14:06
 */

#ifndef DIMENSION_H
#define DIMENSION_H

#define NOMINMAX


#include <algorithm>
#include <type_traits>
#include "utils.hpp"
#include "vector2D.h"


class Dimension {
    
protected:
    Platec::vec2ui dim;
public:

    /// Initialize the dimension with the given values
    Dimension(const uint32_t width,const uint32_t height);

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getArea() const;
    uint32_t indexOf(const Platec::vec2ui& point) const;
    
    uint32_t yFromIndex(const uint32_t index) const;
    uint32_t xFromIndex(const uint32_t index) const;    
    
    const Platec::vec2ui coordOF(const uint32_t index) const;

    template <class T>
    bool contains(const Platec::Vector2D<T>& p) const {
        //using std::floor here to avoid floating point inaccuarcy
       return (std::floor(p.x()) >= 0 && std::floor(p.x()) < getWidth()  
               && std::floor(p.y()) >= 0.0f && std::floor(p.y()) < getHeight());
    }
    void grow(Platec::vec2ui growSize);

    uint32_t getMax() const;
    uint32_t xMod(const uint32_t x) const;
    Platec::vec2ui xMod(const Platec::vec2ui& point) const;
    uint32_t yMod(const uint32_t y) const;
    Platec::vec2ui yMod(const Platec::vec2ui& point) const;
    Platec::vec2ui pointMod(const Platec::vec2ui& point) const;
    Platec::vec2ui  normalize(const Platec::vec2ui& point) const;
    uint32_t lineIndex(const uint32_t y) const;
    uint32_t normalizedIndexOf(const Platec::vec2ui& point) const; 
    uint32_t xCap(const uint32_t x) const;
    Platec::vec2ui xCap(const Platec::vec2ui& point) const;
    uint32_t yCap(const uint32_t y) const;
    Platec::vec2ui yCap(const Platec::vec2ui& point) const;
    
    template <class T>
    Platec::Vector2D<T> wrap(const Platec::Vector2D<T>& point) const
    {
        T xval = point.x(), yval = point.y();
        if(std::floor(xval) < 0)
        {
            xval += static_cast<T>(getWidth());
        }
        else if (std::floor(xval) > getWidth())
        {
            xval -= static_cast<T>(getWidth());
        }
        
        if(std::floor(yval) < 0)
        {
            yval += static_cast<T>(getHeight());
        }
        else if (std::floor(yval) > getHeight())
        {
            yval -= static_cast<T>(getHeight());
        }
        return Platec::Vector2D<T> (xval,yval);
    }

};

#endif /* DIMENSION_H */

