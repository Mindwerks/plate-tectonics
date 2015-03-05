#include "bounds.hpp"

Bounds::Bounds(const WorldDimension& worldDimension, const FloatPoint& position, 
	   const Dimension& dimension)
	: _worldDimension(worldDimension), 
	  _position(position), 
	  _dimension(dimension) {

};

uint32_t Bounds::index(uint32_t x, uint32_t y) const {
	if (x >= _dimension.getWidth()) {
		throw runtime_error("Bounds::Index: unvalid x coordinate");
	}
	if (y >= _dimension.getHeight()) {
		throw runtime_error("Bounds::Index: unvalid y coordinate");
	}
	return y * _dimension.getWidth() + x;
} 	

uint32_t Bounds::area() const{
	return _dimension.getArea();
}

uint32_t Bounds::width() const {
	return _dimension.getWidth();
}

uint32_t Bounds::height() const {
	return _dimension.getHeight();
}

float Bounds::left() const {
	return _position.getX();
}

float Bounds::top() const {
	return _position.getY();
}	

float Bounds::right() const {
	return left() + width() - 1;
}

float Bounds::bottom() const {
	return top() + height() - 1;
}

bool Bounds::contains(uint32_t x, uint32_t y) const {
	uint32_t cleanX = _worldDimension.xMod(x);
	uint32_t cleanY = _worldDimension.yMod(y);
	return cleanX >= _position.getX() && cleanX<(_position.getX() + _dimension.getWidth()) 
    	&& cleanY >= _position.getY() && cleanY<(_position.getY() + _dimension.getHeight());
}

bool Bounds::isInLimits(float x, float y) const {
	if (x<0) return false;
	if (y<0) return false;
	return x<=_dimension.getWidth() && y<=_dimension.getHeight();
}

void Bounds::grow(float dx, float dy) {
	_position.grow(dx, dy, _worldDimension);
	p_assert(_worldDimension.contains(_position), "");
}

void Bounds::growWidth(int d){
	if (d<0) throw runtime_error("negative value");
	_dimension.growWidth(d);
}

void Bounds::growHeight(int d){
	if (d<0) throw runtime_error("negative value");
	_dimension.growHeight(d);
}	

Platec::Rectangle Bounds::asRect() const {
    p_assert(_position.getX() >= 0.0f && _position.getY() >= 0.0f, "Left and top must be positive");  
    const uint32_t ilft = (uint32_t)_position.getX();
    const uint32_t itop = (uint32_t)_position.getY();
    const uint32_t irgt = ilft + _dimension.getWidth();
    const uint32_t ibtm = itop + _dimension.getHeight();

    return Platec::Rectangle(_worldDimension, ilft, irgt, itop, ibtm);     
}

uint32_t Bounds::getMapIndex(uint32_t* px, uint32_t* py) const
{
    return asRect().getMapIndex(px, py);       
}

uint32_t Bounds::getValidMapIndex(uint32_t* px, uint32_t* py) const
{
    uint32_t res = asRect().getMapIndex(px, py);
    if (res == BAD_INDEX) {
        throw runtime_error("BAD INDEX found");
    }
    return res;
}
