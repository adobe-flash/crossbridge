/**
 Author: Renaun Erickson
 **/
#include <math.h>
#include <vector>
#include "micropather.h"
using namespace micropather;

class Pather : public Graph {
private:
    unsigned char *map;
    MicroPather* pather;
    int mapColSize;
    int mapRowSize;
    
public:
    Pather() {
    }
    virtual ~Pather() {
        delete pather;
    };
    void            setMap(unsigned char* buffer, int colSize, int rowSize);
    int*            getPath(int sx, int sy, int nx, int ny);
    
    int             Passable(int nx, int ny);
    void            NodeToXY(void* node, int* x, int* y);
    void*           XYToNode(int x, int y);
    
    virtual float   LeastCostEstimate(void* nodeStart, void* nodeEnd);
    virtual void    AdjacentCost(void* node, std::vector< StateCost > *neighbors);
    virtual void    PrintStateInfo( void* node );
};