#ifndef ObstacleDef
#define ObstacleDef

class Obstacle : public Cell {
public:
  Obstacle(Coordinate &aCoord) : Cell(aCoord) {
    image = ObstacleImage;
  }
};

#endif
