#ifndef OceanDef
#define OceanDef

class Cell;
Cell *cells[MaxRows][MaxCols];

class Ocean {
friend class Cell;
private:
  unsigned numRows;
  unsigned numCols;
  unsigned numPrey;
  unsigned numPredators;
  unsigned numObstacles;
  Random *random;

  void initCells(void);
  void addEmptyCells(void);
  void addObstacles(void);
  void addPredators(void);
  void addPrey(void);
  Coordinate getEmptyCellCoord(void);
  void displayBorder(void);
  void displayCells(void);
  void displayStats(int iteration);

public:
  unsigned getNumPrey(void) {return numPrey;}
  unsigned getNumPredators(void) {return numPredators;}
  void setNumPrey(unsigned aNumber) {numPrey = aNumber;}
  void setNumPredators(unsigned aNumber) {numPredators = aNumber;}
  void initialize(void);
  void run(void);
  Ocean(void) {}
  ~Ocean(void) {delete random;}
};

Ocean *Ocean1;

#endif
