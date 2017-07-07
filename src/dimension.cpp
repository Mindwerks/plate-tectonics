/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Dimension.cpp
 * Author: ro-lip
 * 
 * Created on 16. Mai 2017, 14:06
 */

#include "dimension.h"


Dimension::Dimension(uint32_t width, uint32_t height) :
dim(width, height) {
}

Platec::vec2ui Dimension::getDimensions() const {
    return dim;
}

void Dimension::grow(Platec::Vector2D<uint32_t> growSize) {
    dim = dim+growSize;
}

uint32_t Dimension::getWidth() const {
    return dim.x();
}


uint32_t Dimension::getHeight() const {
    return dim.y();
}


uint32_t Dimension::getArea() const {
    return getWidth() * getHeight();
}


uint32_t Dimension::getMax() const {
    return std::max(getWidth(), getHeight());
}

uint32_t Dimension::xMod(const uint32_t x) const {
    return x >= getWidth() ? x-getWidth() : x;
}

uint32_t Dimension::yMod(const uint32_t y) const {
    return y >= getHeight() ? y-getHeight() : y;
}

Platec::vec2ui  Dimension::normalize
                    (const Platec::vec2ui& point) const {
    return yMod(xMod(point));
}


uint32_t Dimension::indexOf(const Platec::vec2ui& point) const {
    return point.y() * getWidth() + point.x();
}

const Platec::vec2ui
                    Dimension::coordOF(const uint32_t index) const {
    return Platec::vec2ui(xFromIndex(index), yFromIndex(index));
}




uint32_t Dimension::lineIndex(const uint32_t y) const {
    ASSERT(y < getHeight(), "y is not valid");
    return indexOf(Platec::vec2ui(0, y));
}

uint32_t Dimension::yFromIndex(const uint32_t index) const {
    return index / getWidth();
}

uint32_t Dimension::xFromIndex(const uint32_t index) const {
    return index - yFromIndex(index) * getWidth();
}

uint32_t Dimension::xCap(const uint32_t x) const {
    return std::min(x, getWidth()-1);
}

uint32_t Dimension::yCap(const uint32_t y) const {
    return std::min(y, getHeight()-1);
}

Platec::vec2ui Dimension::xMod
                            (const Platec::vec2ui& point) const {
    if (point.x()>= getWidth()) {
        return Platec::vec2ui(point.x()-getWidth(), point.y());
    }
    return point;
}

Platec::vec2ui Dimension::yMod
                        (const Platec::vec2ui& point) const {
    if (point.y()>= getHeight()) {
        return Platec::vec2ui(point.x(), point.y()-getHeight());
    }
    return point;
}

Platec::vec2ui Dimension::pointMod
                        (const Platec::vec2ui& point) const {
    return Platec::vec2ui(xMod(point.x()), yMod(point.y()));
}


uint32_t Dimension::normalizedIndexOf
                    (const Platec::vec2ui& point) const {
    return indexOf( pointMod( point));
}

Platec::vec2ui Dimension::xCap
                        (const Platec::vec2ui& point) const {
    return Platec::vec2ui (std::min(point.x(), getWidth()-1), point.y());
}

Platec::vec2ui Dimension::yCap
                            (const Platec::vec2ui& point) const {
    return Platec::vec2ui (point.x(), std::min(point.y(), getHeight()-1));
}

