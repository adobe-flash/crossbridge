/**
    Author: Renaun Erickson
**/
#include "pathfinder.h"

void Pather::setMap(unsigned char* buffer, int colSize, int rowSize) {
    map = buffer;
    mapColSize = colSize;
    mapRowSize = rowSize;
    int memSize = mapColSize * mapRowSize;
    if (memSize > 1000) memSize /= 4;
    pather = new MicroPather(this, memSize);
}

int* Pather::getPath( int sx, int sy, int nx, int ny )
{
    // micropather uses this
    std::vector<void*> path;
    // I want to change to my own format and return with pathResult and size of path, could add total cost too, possible memory leak
    unsigned short int* result;
    int pathResult = MicroPather::NO_SOLUTION;
    unsigned int size = 0;
    if ( Passable( sx, sy ) == 1 && Passable( nx, ny ) == 1 )
    {
        float totalCost;
        pathResult = pather->Solve( XYToNode( sx, sy ), XYToNode( nx, ny ), &path, &totalCost );
    }
    
    if ( pathResult == MicroPather::SOLVED ) {
        size = path.size();
        result = new unsigned short int[(size*2)+2];
        result[0] = pathResult;
        result[1] = size;
        unsigned k;
        for( k=0; k<size; ++k ) {
            int x, y;
            NodeToXY( path[k], &x, &y );
            result[(k*2)+2] = x;
            result[(k*2)+3] = y;
        }
    }
    else
    {
        result = new unsigned short int[2];
        result[0] = pathResult;
        result[1] = size;
    }
    return (int*)result;
}

int Pather::Passable( int nx, int ny )
{
    if ( nx >= 0 && nx < mapColSize
        && ny >= 0 && ny < mapRowSize )
    {
        int index = ny*mapColSize+nx;
        return map[ index ];
    }		
    return 0;
}

void Pather::NodeToXY( void* node, int* x, int* y )
{
    int index = (intptr_t)node;
    *y = index / mapColSize;
    *x = index - *y * mapColSize;
}

void* Pather::XYToNode( int x, int y )
{
    return (void*) ( y*mapColSize + x );
}

float Pather::LeastCostEstimate( void* nodeStart, void* nodeEnd )
{
    int xStart, yStart, xEnd, yEnd;
    NodeToXY( nodeStart, &xStart, &yStart );
    NodeToXY( nodeEnd, &xEnd, &yEnd );
    
    /* Compute the minimum path cost using distance measurement. It is possible
     to compute the exact minimum path using the fact that you can move only
     on a straight line or on a diagonal, and this will yield a better result.
     */
    int dx = xStart - xEnd;
    int dy = yStart - yEnd;
    return (float) sqrt( (double)(dx*dx) + (double)(dy*dy) );
}

void Pather::AdjacentCost( void* node, std::vector< StateCost > *neighbors )
{
    int x, y;
    const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
    const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    const float cost[8] = { 1.0f, 1.41f, 1.0f, 1.41f, 1.0f, 1.41f, 1.0f, 1.41f };
    
    NodeToXY( node, &x, &y );
    
    for( int i=0; i<8; ++i ) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        
        int pass = Passable( nx, ny );
        if ( pass == 1 )
        {
            // Normal floor
            StateCost nodeCost = { XYToNode( nx, ny ), cost[i] };
            neighbors->push_back( nodeCost );
        }
    }
}

void Pather::PrintStateInfo( void* node )
{
}