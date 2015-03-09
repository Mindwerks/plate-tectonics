#include "plate.hpp"
#include "gtest/gtest.h"

TEST(CreatePlate, SquareDoesNotExplode)
{
  const float *heightmap = new float[40000]; // 200 x 200
  plate p = plate(123, heightmap, 100, 3, 50, 23, 18, WorldDimension(200, 200));
}

TEST(CreatePlate, NotSquareDoesNotExplode)
{
  const float *heightmap = new float[80000]; // 200 x 400
  plate p = plate(123, heightmap, 100, 3, 50, 23, 18, WorldDimension(200, 400));
}

// TODO test also when plate is large as world
TEST(Plate, calculateCrust)
{
  const float *heightmap = new float[256 * 128];
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
    virtual void incArea() { throw runtime_error("Not implemented"); }
    virtual void enlarge_to_contain(uint32_t x, uint32_t y) { throw runtime_error("Not implemented"); }
    virtual void markNonExistent() { throw runtime_error("Not implemented"); }
    virtual void shift(uint32_t dx, uint32_t dy) { throw runtime_error("Not implemented"); }

    virtual uint32_t getLeft() const { throw runtime_error("Not implemented"); }
    virtual uint32_t getRight() const { throw runtime_error("Not implemented"); }
    virtual uint32_t getTop() const { throw runtime_error("Not implemented"); }
    virtual uint32_t getBottom() const { throw runtime_error("Not implemented"); }
    virtual bool isEmpty() const { throw runtime_error("Not implemented"); }
    virtual uint32_t area() const { return _area; }
    virtual uint32_t collCount() const { return _collCount; }
private:
    uint32_t _collCount;
    uint32_t _area;
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
    virtual uint32_t size() const { throw runtime_error("Not implemented"); }
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
    virtual const ContinentId& id(uint32_t index) const { throw runtime_error("Not implemented"); }
    virtual ContinentId& id(uint32_t index) { throw runtime_error("Not implemented"); }
    virtual void setId(uint32_t index, ContinentId id) const { throw runtime_error("Not implemented"); }
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
  const float *heightmap = new float[256 * 128];
  plate p = plate(123, heightmap, 100, 3, 50, 23, 18, WorldDimension(256, 128));  

  MockSegmentData* mSeg = new MockSegmentData(7, 789);
  MockSegments* mSegments = new MockSegments(IntPoint(123, 78), 99, mSeg);
  p.injectSegments(mSegments);

  uint32_t area = p.addCollision(123, 78);

  EXPECT_EQ(789, area);
  EXPECT_EQ(8, mSeg->collCount());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}