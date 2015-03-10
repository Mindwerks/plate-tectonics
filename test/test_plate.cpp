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

#include "plate.hpp"
#include "gtest/gtest.h"
#include "noise.hpp"

void initializeHeightmapWithNoise(long seed, float *heightmap, const WorldDimension& wd)
{
  createNoise(heightmap, wd, SimpleRandom(seed), true);
  for (int i=0; i<wd.getArea(); i++){
    if (heightmap[i]<0.0f){
      heightmap[i] *= -1.0f;
    }
  }
}

TEST(Noise, Repeatability)
{
  WorldDimension wd(233, 111);
  float *heightmap = new float[wd.getArea()];
  initializeHeightmapWithNoise(123, heightmap, wd);

  EXPECT_FLOAT_EQ(1.4012985e-45f, heightmap[0]);
  EXPECT_FLOAT_EQ(0.525886f, heightmap[1000]);
  EXPECT_FLOAT_EQ(0.3915835f, heightmap[2000]);
  EXPECT_FLOAT_EQ(0.45404199f, heightmap[5000]);
  EXPECT_FLOAT_EQ(0.64141226f, heightmap[8000]);
  EXPECT_FLOAT_EQ(0.5614078f, heightmap[11000]);
  EXPECT_FLOAT_EQ(0.57266176f, heightmap[13000]);

  delete[] heightmap;
}

TEST(CreatePlate, SquareDoesNotExplode)
{
  float *heightmap = new float[40000]; // 200 x 200
  initializeHeightmapWithNoise(678, heightmap, WorldDimension(200, 200));
  plate p = plate(123, heightmap, 100, 3, 50, 23, 18, WorldDimension(200, 200));
}

TEST(CreatePlate, NotSquareDoesNotExplode)
{
  float *heightmap = new float[80000]; // 200 x 400
  initializeHeightmapWithNoise(678, heightmap, WorldDimension(200, 400));
  plate p = plate(123, heightmap, 100, 3, 50, 23, 18, WorldDimension(200, 400));
}

// TODO test also when plate is large as world
TEST(Plate, calculateCrust)
{
  float *heightmap = new float[256 * 128];
  initializeHeightmapWithNoise(678, heightmap, WorldDimension(256, 128));
  plate p = plate(123, heightmap, 100, 3, 50, 23, 18, WorldDimension(256, 128));	
  uint32_t x, y, index;
  float w_crust, e_crust, n_crust, s_crust;
  uint32_t w, e, n, s;

  // top left corner
  x = 0;
  y = 0;
  index = 1;
  p.calculateCrust(x,y,index,w_crust,e_crust,n_crust,s_crust,w,e,n,s);
  EXPECT_EQ(0, w);
  EXPECT_EQ(1, e);
  EXPECT_EQ(0, n);
  EXPECT_EQ(100, s);

  // bottom right corner
  x = 99;
  y = 2;
  index = 1;
  p.calculateCrust(x,y,index,w_crust,e_crust,n_crust,s_crust,w,e,n,s);
  EXPECT_EQ(298, w);
  EXPECT_EQ(200, e);
  EXPECT_EQ(199, n);
  EXPECT_EQ(99, s);

  // point in the middle
  x = 50;
  y = 1;
  index = 1;
  p.calculateCrust(x,y,index,w_crust,e_crust,n_crust,s_crust,w,e,n,s);
  EXPECT_EQ(149, w);
  EXPECT_EQ(151, e);
  EXPECT_EQ(50, n);
  EXPECT_EQ(250, s); 
}

class MockSegmentData : public ISegmentData
{
public:
    MockSegmentData(uint32_t collCount, uint32_t area) 
      : _collCount(collCount), _area(area)
    {

    }
    virtual void incCollCount() { _collCount++; }
    virtual void incArea() { _area++; }
    virtual void enlarge_to_contain(uint32_t x, uint32_t y) { 
      _enlargePoint = new IntPoint(x, y);
    }
    virtual void markNonExistent() { throw runtime_error("Not implemented"); }
    virtual void shift(uint32_t dx, uint32_t dy) { throw runtime_error("Not implemented"); }

    virtual uint32_t getLeft() const { throw runtime_error("Not implemented"); }
    virtual uint32_t getRight() const { throw runtime_error("Not implemented"); }
    virtual uint32_t getTop() const { throw runtime_error("Not implemented"); }
    virtual uint32_t getBottom() const { throw runtime_error("Not implemented"); }
    virtual bool isEmpty() const { throw runtime_error("Not implemented"); }
    virtual uint32_t area() const { return _area; }
    virtual uint32_t collCount() const { return _collCount; }
    IntPoint* enlargedPoint() { return _enlargePoint; }
private:
    uint32_t _collCount;
    uint32_t _area;
    IntPoint* _enlargePoint;
};

class MockSegments : public ISegments
{
public:
    MockSegments(IntPoint p, ContinentId id, ISegmentData* data)
      : _p(p), _id(id), _data(data)
    {

    }
    virtual uint32_t area() { throw runtime_error("Not implemented"); }
    virtual void reset() { throw runtime_error("Not implemented"); }
    virtual void reassign(uint32_t newarea, uint32_t* tmps) { throw runtime_error("Not implemented"); }
    virtual void shift(uint32_t d_lft, uint32_t d_top) { throw runtime_error("Not implemented"); }
    virtual uint32_t size() const { throw runtime_error("(MockSegments::size) Not implemented"); }
    virtual const ISegmentData& operator[](uint32_t index) const { 
      if (index == _id) {
        return *_data;
      } else {
        throw runtime_error("(MockSegments::operator[]) Unexpected call"); 
      }
    }
    virtual ISegmentData& operator[](uint32_t index) {
      if (index == _id) {
        return *_data;
      } else {
        throw runtime_error("(MockSegments::operator[]) Unexpected call"); 
      }
    }
    virtual void add(ISegmentData* data) { throw runtime_error("Not implemented"); }
    virtual const ContinentId& id(uint32_t index) const { 
      throw runtime_error("(MockSegments::id) Not implemented"); 
    }
    virtual ContinentId& id(uint32_t index) { throw runtime_error("(MockSegments::id) Not implemented"); }
    virtual void setId(uint32_t index, ContinentId id) { throw runtime_error("Not implemented"); }
    virtual ContinentId getContinentAt(int x, int y) const { 
      if (x==_p.getX() && y==_p.getY()){        
        return _id;
      } else {
        throw runtime_error("(MockSegments::getContinentAt) Unexpected call");
      }
    }
private:
    IntPoint _p;
    ContinentId _id;
    ISegmentData* _data;      
};

TEST(Plate, addCollision)
{
  float *heightmap = new float[256 * 128];
  initializeHeightmapWithNoise(678, heightmap, WorldDimension(256, 128));
  plate p = plate(123, heightmap, 100, 3, 50, 23, 18, WorldDimension(256, 128)); 

  MockSegmentData* mSeg = new MockSegmentData(7, 789);
  MockSegments* mSegments = new MockSegments(IntPoint(123, 78), 99, mSeg);
  p.injectSegments(mSegments);

  uint32_t area = p.addCollision(123, 78);

  EXPECT_EQ(789, area);
  EXPECT_EQ(8, mSeg->collCount());

  delete[] heightmap;
}

class MockSegments2 : public ISegments
{
public:
    MockSegments2(IntPoint p, ContinentId id, ISegmentData* data, uint32_t index)
      : _p(p), _id(id), _data(data), _index(index)
    {

    }
    virtual uint32_t area() { throw runtime_error("(MockSegments2::area) Not implemented"); }
    virtual void reset() { throw runtime_error("(MockSegments2::reset) Not implemented"); }
    virtual void reassign(uint32_t newarea, uint32_t* tmps) { throw runtime_error("(MockSegments2::reassign) Not implemented"); }
    virtual void shift(uint32_t d_lft, uint32_t d_top) { throw runtime_error("(MockSegments2::shift) Not implemented"); }
    virtual uint32_t size() const { throw runtime_error("(MockSegments2::size) Not implemented"); }
    virtual const ISegmentData& operator[](uint32_t index) const { 
      if (index == _id) {
        return *_data;
      } else {
        throw runtime_error("(MockSegments2::operator[]) Unexpected call"); 
      }
    }
    virtual ISegmentData& operator[](uint32_t id) {
      if (id == _id) {
        return *_data;
      } else {
        throw runtime_error(string("(MockSegments2::operator[]) Unexpected call with id "
          + Platec::to_string(id))); 
      }
    }
    virtual void add(ISegmentData* data) { throw runtime_error("(MockSegments2::add) Not implemented"); }
    virtual const ContinentId& id(uint32_t index) const { 
      if (_index == index) return _id;
      throw runtime_error(
        string("(MockSegments2::id) Unexpected value ")
        + Platec::to_string(index)
        + " expected was "
        + Platec::to_string(_index)); 
    }
    virtual ContinentId& id(uint32_t index) { 
      if (_index == index) return _id;
      throw runtime_error(
        string("(MockSegments2::id) Unexpected value ")
        + Platec::to_string(index)
        + " expected was "
        + Platec::to_string(_index)); 
    }
    virtual void setId(uint32_t index, ContinentId id) { 
      if (_index == index) {
        _id = id;
      } else {
        throw runtime_error(string("(MockSegments2::setId) Unexpected call with index ")
          + Platec::to_string(index)); 
      }
    }
    virtual ContinentId getContinentAt(int x, int y) const { 
      if (x==_p.getX() && y==_p.getY()){        
        return _id;
      } else {
        throw runtime_error("(MockSegments2::getContinentAt) Unexpected call");
      }
    }
private:
    IntPoint _p;
    ContinentId _id;
    ISegmentData* _data;
    uint32_t _index;      
};

TEST(Plate, addCrustByCollision)
{
  const uint32_t worldWidth = 256;
  const uint32_t worldHeight = 128;

  float *heightmap = new float[worldWidth * worldHeight];
  WorldDimension wd(worldWidth, worldHeight);
  initializeHeightmapWithNoise(1, heightmap, wd);
  
  // Suppose the plate start at 170, 70 and ends at 250, 125
  plate p = plate(123, heightmap, 80, 55, 170, 70, 18, WorldDimension(256, 128));
  // the point of collision in world coordinates
  const uint32_t worldPointX = 240;
  const uint32_t worldPointY = 120;
  // the point of collision in plate coordinates
  const uint32_t platePointX = 70;
  const uint32_t platePointY = 50;
  uint32_t indexInPlate = platePointY * 80 + platePointX;
  
  MockSegmentData* mSeg = new MockSegmentData(7, 789);
  MockSegments2* mSegments = new MockSegments2(IntPoint(worldPointX, worldPointY), 99, mSeg, indexInPlate);
  p.injectSegments(mSegments);

  uint32_t timestampIn_240_120before = p.getCrustTimestamp(worldPointX, worldPointY);
  float crustIn_240_120before = p.getCrust(worldPointX, worldPointY);

  // Assumptions:
  // the point is in the plate bounds 

  p.addCrustByCollision(
    worldPointX, worldPointY, // Point of impact
    0.8, // Amount of crust
    123, // Current age
    99); // Active continent

  // Age of the point should be updated
  uint32_t timestampIn_240_120after = p.getCrustTimestamp(worldPointX, worldPointY);
  ASSERT_EQ(true, timestampIn_240_120after > timestampIn_240_120before);
  ASSERT_EQ(true, timestampIn_240_120after < 123 );

  // Crust should be increased
  float crustIn_240_120after = p.getCrust(worldPointX, worldPointY);
  EXPECT_FLOAT_EQ(crustIn_240_120before + 0.8, crustIn_240_120after);  
  
  // The activeContinent should now owns the point
  EXPECT_EQ(99, mSegments->getContinentAt(worldPointX, worldPointY));

  // The activeContinent should contains the point
  ASSERT_EQ(false, NULL==mSeg->enlargedPoint());
  EXPECT_EQ(70, mSeg->enlargedPoint()->getX());
  EXPECT_EQ(50, mSeg->enlargedPoint()->getY());

  // The activeContinent are should be increased
  EXPECT_EQ(790, mSeg->area());
  
  delete[] heightmap;
}

TEST(Plate, addCrustBySubduction)
{
  const uint32_t worldWidth = 256;
  const uint32_t worldHeight = 128;

  float *heightmap = new float[worldWidth * worldHeight];
  initializeHeightmapWithNoise(1, heightmap, WorldDimension(worldWidth, worldHeight));
  
  // Suppose the plate start at 170, 70 and ends at 250, 125
  plate p = plate(123, heightmap, 80, 55, 170, 70, 18, WorldDimension(256, 128));
  // the point of collision in world coordinates
  const uint32_t worldPointX = 240;
  const uint32_t worldPointY = 120;
  // the point of collision in plate coordinates
  const uint32_t platePointX = 70;
  const uint32_t platePointY = 50;
  uint32_t indexInPlate = platePointY * 80 + platePointX;
  
  MockSegmentData* mSeg = new MockSegmentData(7, 789);
  MockSegments2* mSegments = new MockSegments2(IntPoint(worldPointX, worldPointY), 99, mSeg, indexInPlate);
  p.injectSegments(mSegments);

  uint32_t timestampIn_240_120before = p.getCrustTimestamp(worldPointX, worldPointY);
  float crustIn_240_120before = p.getCrust(worldPointX, worldPointY);

  float massBefore = p.getMass();

  // Assumptions:
  // the point is in the plate bounds 
  float dx = 0.0f;
  float dy = 0.0f;
  p.addCrustBySubduction(
    worldPointX, worldPointY, // Point of impact
    0.8, // Amount of crust
    123, // Current age
    dx, dy); // Direction of the subducting plate

  // Crust should be increased
  float crustIn_240_120after = p.getCrust(worldPointX, worldPointY);
  EXPECT_FLOAT_EQ(crustIn_240_120before + 0.8, crustIn_240_120after);  

  // The mass should be increased
  float massAfter = p.getMass();
  EXPECT_EQ(massBefore + 0.8f, massAfter);

  // Age of the point should be updated
  uint32_t timestampIn_240_120after = p.getCrustTimestamp(worldPointX, worldPointY);
  ASSERT_EQ(true, timestampIn_240_120after > timestampIn_240_120before);
  ASSERT_EQ(true, timestampIn_240_120after < 123 );
  
  delete[] heightmap;  
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}