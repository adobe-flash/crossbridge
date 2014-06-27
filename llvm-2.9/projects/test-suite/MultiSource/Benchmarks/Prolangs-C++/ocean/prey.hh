#ifndef PreyDef
#define PreyDef

class Prey : public Cell {
protected:
  int timeToReproduce;

  void moveFrom(Coordinate from, Coordinate to);
  Cell *reproduce(Coordinate anOffset);

public:
  Prey(Coordinate &aCoord) : Cell(aCoord) {
    timeToReproduce = TimeToReproduce;
    image = DefaultPreyImage;
  }

  void process(void) {
    Coordinate toCoord;

    toCoord = getEmptyNeighborCoord();
    moveFrom(*offset,toCoord);
  }
};

#endif
