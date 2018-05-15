/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Vector2D.h
 * Author: ro-lip
 *
 * Created on 15. Mai 2017, 15:03
 */

#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>
#include <type_traits>
#include <stdint.h>

namespace Platec {
   
template<class T, class Enable = void>
class Vector2D {};
    
template<class T>  
class Vector2D<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
{
private:    
    T x_value, y_value;
public:
    Vector2D(T x, T y) : x_value(x), y_value(y) {}
    T x() const {
        return x_value;
    }
    T y() const {
        return y_value;
    }
    float_t length() const {
        return std::hypot(static_cast<float_t>(x_value),
                static_cast<float_t>(y_value));
    }
    
    friend Vector2D<T> operator-(const Vector2D<T>& a, const Vector2D<T>& b) {
        return Vector2D<T>(a.x() - b.x(), a.y() - b.y());
    }
    
    T dotProduct(const Vector2D<T>& other) const {
        return x() * other.x() + y() * other.y();
    }
    
    friend bool operator==(const Vector2D<T>& a, const Vector2D<T>& b) {
        return a.x() == b.x() && a.y() == b.y();
    }
    
    friend Vector2D<T> operator+(const Vector2D<T>& a, const Vector2D<T>& b) {
        return Vector2D<T>(a.x() + b.x(), a.y() + b.y());
    }
    
    friend Vector2D<T> operator*(const Vector2D<T>& v, T f) {
        return Vector2D<T>(v.x() * f, v.y() * f);
    }
    
    friend Vector2D<T> operator/(const Vector2D<T>& v, T f) {
        return Vector2D<T>(v.x() / f, v.y() / f);
    }    
    
    void shift(const Vector2D<T>& delta) {
        x_value += delta.x();
        y_value += delta.y();
    } 
    
    Vector2D<T> getTopPosition() const
    {
        return Vector2D<T>(x_value, y_value-1);
    }
    
    Vector2D<T> getBottomPosition() const
    {
        return Vector2D<T>(x_value, y_value+1);
    }
    
     Vector2D<T> getLeftPosition() const
    {
        return Vector2D<T>(x_value-1, y_value);
    }
     
     Vector2D<T> getRightPosition() const
    {
        return Vector2D<T>(x_value+1, y_value);
    }     
};

typedef Vector2D<float_t> vec2f;
typedef Vector2D<uint32_t> vec2ui;

}

#endif /* VECTOR2D_H */

