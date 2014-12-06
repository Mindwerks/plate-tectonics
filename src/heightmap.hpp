#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include <stdexcept> // std::invalid_argument

using namespace std;

template <typename Value>
class Matrix
{
	public:

    Matrix(unsigned int width, unsigned int height)
        : _width(width), _height(height)
    {
        if (width == 0 || height == 0) {
            throw invalid_argument("width and height should be greater than zero");
        }
        _data = new Value[width * height];
    };

    Matrix( const Matrix<Value>& other )
        : _width(other._width), _height(other._height)
    {
        _data = new Value[_width * _height];
        for (int x=0; x<_width;x++){
            for (int y=0; y<_height;y++){
                set(x,y,other.get(x,y));
            }
        }
    }

    ~Matrix()
    {
        delete _data;
    }

    const Matrix<Value> copy()
    {
        Matrix<Value> copied(_width, _height);
        memcpy(copied._data, _data, sizeof(Value) * _width * _height);
        return copied;
    }

    const void set_all(const Value& value)
    {
        for (int x=0; x<_width;x++){
            for (int y=0; y<_height;y++){
                set(x,y,value);
            }
        }
    }

    inline const Value& set(unsigned int x, unsigned y, const Value& value)
    {
        if (x >= _width || y >= _height) {
            throw invalid_argument("invalid coordinates");
        }
        _data[y * _width + x] = value;
        return value;
    }

    inline Value get(unsigned int x, unsigned y) const
    {
        if (x >= _width || y >= _height) {
            throw invalid_argument("invalid coordinates");
        }
        return _data[y * _width + x];
    }

    Matrix<Value> const& operator=(Matrix<Value> const& dst)
    {
        _width = dst._width;
        _height = dst._height;
        memcpy(_data, dst._data, sizeof(Value) * _width * _height);
        return dst;
    }

    Value& operator[](unsigned int index) const
    {
        if (index >= (_width*_height)) {
            throw invalid_argument("invalid index");
        }
        return this->_data[index];
    }

	private:

    Value* _data;
    unsigned int _width;
    unsigned int _height;
};

typedef Matrix<float> HeightMap;

#endif
