#ifndef CoordinateDef
#define CoordinateDef

class Coordinate {
private:
  unsigned x;
  unsigned y;

public:
  Coordinate(unsigned anX, unsigned aY) : x(anX), y(aY) {}
  Coordinate(const Coordinate &aCoord) {
    x = aCoord.x;
    y = aCoord.y;
  }
  Coordinate(void) {}
  ~Coordinate(void) {}
  unsigned getX(void) {return x;}
  unsigned getY(void) {return y;}
  void setX(unsigned anX) {x = anX;}
  void setY(unsigned aY) {y = aY;}
  void operator = (const Coordinate &aCoord) {
    x = aCoord.x;
    y = aCoord.y;
  }
  int operator == (const Coordinate &c) {
    return (x == c.x && y == c.y);
  }
  int operator != (const Coordinate &c) {
    return (x != c.x && y != c.y);
  }
};

#endif
