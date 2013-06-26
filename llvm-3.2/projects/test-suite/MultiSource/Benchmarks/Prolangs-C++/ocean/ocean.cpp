#include "constants.hh"
#include "random.hh"
#include "coordinate.hh"
#include "ocean.hh"
#include "cell.hh"
#include "prey.hh"
#include "predator.hh"
#include "obstacle.hh"

#include <cstdlib>

//cell.cc
Cell *Cell::getCellAt(Coordinate aCoord) {
  return cells[aCoord.getY()][aCoord.getX()];
}

void Cell::assignCellAt(Coordinate aCoord, Cell *aCell) {
  cells[aCoord.getY()][aCoord.getX()] = aCell;
}

Cell *Cell::getNeighborWithImage(char anImage) {
  Cell *neighbors[4];
  unsigned count = 0;
  if (north()->getImage() == anImage) neighbors[count++] = north();
  if (south()->getImage() == anImage) neighbors[count++] = south();
  if (east()->getImage() == anImage) neighbors[count++] = east();
  if (west()->getImage() == anImage) neighbors[count++] = west();
  if (!count) return this;
  else
    return neighbors[Ocean1->random->nextIntBetween(0,count-1)];
}

Coordinate Cell::getEmptyNeighborCoord(void) {
  return getNeighborWithImage(DefaultImage)->getOffset();
}

Coordinate Cell::getPreyNeighborCoord(void) {
  return getNeighborWithImage(DefaultPreyImage)->getOffset();
}

Cell *Cell::north(void) {
  unsigned yvalue;
  yvalue = (offset->getY() > 0) ? (offset->getY()-1) : (Ocean1->numRows-1);
  return cells[yvalue][offset->getX()];
}

Cell *Cell::south(void) {
  unsigned yvalue;
  yvalue = (offset->getY()+1) % Ocean1->numRows;
  return cells[yvalue][offset->getX()];
}

Cell *Cell::east(void) {
  unsigned xvalue;
  xvalue = (offset->getX()+1) % Ocean1->numCols;
  return cells[offset->getY()][xvalue];
}

Cell *Cell::west(void) {
  unsigned xvalue;
  xvalue = (offset->getX() > 0) ? (offset->getX()-1) : (Ocean1->numCols-1);
  return cells[offset->getY()][xvalue];
}

Cell *Cell::reproduce(Coordinate anOffset) {
  Cell *temp = new Cell(anOffset);
  return temp;
}

void Cell::display(void) {}

//prey.cc
void Prey::moveFrom(Coordinate from, Coordinate to) {
  Cell *toCell;
  --timeToReproduce;
  if (to != from) {
    toCell = getCellAt(to);
    delete toCell;
    setOffset(to);
    assignCellAt(to, this);
    if (timeToReproduce <= 0) {
      timeToReproduce = TimeToReproduce;
      assignCellAt(from, reproduce(from));
    }
    else
      assignCellAt(from, new Cell(from));
  }
}

Cell *Prey::reproduce(Coordinate anOffset) {
  Prey *temp = new Prey(anOffset);
  Ocean1->setNumPrey(Ocean1->getNumPrey()+1);
  return (Cell *) temp;
}

//predator.cc
void Predator::process(void) {
  Coordinate toCoord;
  if (--timeToFeed <= 0) {
    assignCellAt(*offset, new Cell(*offset));
    Ocean1->setNumPredators(Ocean1->getNumPredators()-1);
    delete this;
  }
  else {
    toCoord = getPreyNeighborCoord();
    if (toCoord != *offset) {
      Ocean1->setNumPrey(Ocean1->getNumPrey()-1);
      timeToFeed = TimeToFeed;
      moveFrom(*offset, toCoord);
    }
    else
      Prey::process();
  }
}

Cell *Predator::reproduce(Coordinate anOffset) {
  Predator *temp = new Predator(anOffset);
  Ocean1->setNumPredators(Ocean1->getNumPredators()+1);
  return (Cell *) temp;
}

//random.cc
#define MAX 32767

float Random::randReal(void) {
  return random()/(float)RAND_MAX;
}

unsigned Random::nextIntBetween(int low, int high) {
  return (long long)(random())* high / RAND_MAX;
}

//ocean.cc
void Ocean::initialize(void) {
  random = new Random();
  random->initialize();
  numRows = MaxRows;
  numCols = MaxCols;

  numObstacles = DefaultNumObstacles;
  numPredators = DefaultNumPredators;
  numPrey = DefaultNumPrey;

  initCells();
}

void Ocean::initCells(void) {
  addEmptyCells();
  addObstacles();
  addPredators();
  addPrey();
  displayStats(-1);
  displayBorder();
  Ocean1 = this;
}

void Ocean::addEmptyCells(void) {
  for (unsigned row = 0; row < numRows; row++)
    for (unsigned col = 0; col < numCols; col++)
      cells[row][col] = new Cell(Coordinate(col,row));
}

void Ocean::addObstacles(void) {
  Coordinate empty;
  for (unsigned count = 0; count < numObstacles; count++) {
    empty = getEmptyCellCoord();
    cells[empty.getY()][empty.getX()] = new Obstacle(empty);
  }
}

void Ocean::addPredators(void) {
  Coordinate empty;
  for (unsigned count = 0; count < numPredators; count++) {
    empty = getEmptyCellCoord();
    cells[empty.getY()][empty.getX()] = new Predator(empty);
  }
}

void Ocean::addPrey(void) {
  Coordinate empty;
  for (unsigned count = 0; count < numPredators; count++) {
    empty = getEmptyCellCoord();
    cells[empty.getY()][empty.getX()] = new Prey(empty);
  }
}

Coordinate Ocean::getEmptyCellCoord(void) {
  unsigned x, y;
  Coordinate empty;
  do {
    x = random->nextIntBetween(0,numCols-1);
    y = random->nextIntBetween(0,numRows-1);
  }
  while (cells[y][x]->getImage() != DefaultImage);
  empty = cells[y][x]->getOffset();
  delete cells[y][x];
  return empty;
}

void Ocean::displayBorder(void) {
  for (unsigned col = 0; col < numCols; col++);
}

void Ocean::displayCells(void) {
  for (unsigned row = 0; row < numRows; row++)
    for (unsigned col = 0; col < numCols; col++)
      cells[row][col]->display();
}

void Ocean::displayStats(int iteration) {
  // lots of cout stuff
  displayBorder();
}

void Ocean::run(void) {
  unsigned numIterations = DefaultNumIterations; // instead of cin
  if (numIterations > 1000) numIterations = DefaultNumIterations;
  for (unsigned iteration = 0; iteration < numIterations; iteration++) {
    if (numPredators > 0 && numPrey > 0) {
      for (unsigned row = 0; row < numRows; row++)
	for (unsigned col = 0; col < numCols; col++)
	  cells[row][col]->process();
      displayStats(iteration);
      displayCells();
      displayBorder();
    }
  }
}

int main() {
  Ocean *myOcean = new Ocean;
  myOcean->initialize();
  myOcean->run();
}
