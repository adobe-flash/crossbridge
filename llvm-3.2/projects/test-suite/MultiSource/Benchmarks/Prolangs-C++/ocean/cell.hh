#ifndef CellDef
#define CellDef

class Cell {
friend class Ocean;

protected:
  Coordinate *offset;
  char image;

  Cell *getCellAt(Coordinate aCoord);
  void assignCellAt(Coordinate aCoord, Cell *aCell);
  Cell *getNeighborWithImage(char anImage);
  Coordinate getEmptyNeighborCoord(void);
  Coordinate getPreyNeighborCoord(void);
  Cell *north(void);
  Cell *south(void);
  Cell *east(void);
  Cell *west(void);
  virtual Cell *reproduce(Coordinate anOffset);

public:
  Cell(const Coordinate &aCoord) {
    offset = new Coordinate(aCoord);
    image = DefaultImage;
  }
  Cell(void) {}
  virtual ~Cell(void) {delete offset;}
  Coordinate &getOffset(void) {return *offset;}
  void setOffset(Coordinate &anOffset) {offset = new Coordinate(anOffset);}
  char getImage(void) { return image;}
  void display(void);
  virtual void process(void) {}
};

#endif
