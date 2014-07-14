// roadlet.cc

#include <iostream>
#include "roadlet.h"

void roadlet::init(const char *n)
{
  occupant = 0;
  neighbors[0] = neighbors[1] = neighbors[2] = neighbors[3] = 0;
  neighbors[4] = neighbors[5] = neighbors[6] = neighbors[7] = 0;

    name = n; 
    for(int i=0;i<8;i++) 
	the_moves[i] = return_null; 
}


std::ostream& operator<<(std::ostream& o, roadlet r)
{
    o << "roadlet " << r.name;
    return (o);
}
