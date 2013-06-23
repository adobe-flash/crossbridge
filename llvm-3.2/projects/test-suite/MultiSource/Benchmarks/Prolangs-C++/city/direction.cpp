// direction.cc

#include "direction.h"

const char *direction::as_string()
{
    const char *dirs[] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW", 
			   "No direction"};
    return(dirs[dir]);
}
int operator== (direction d1, direction d2)
{
    return (d1.dir == d2.dir);
}
int operator!= (direction d1, direction d2)
{
    return (d1.dir != d2.dir);
}


int operator<= (direction d1, direction d2)
{
    return (d1.dir <= d2.dir);
}


ostream& operator<< (ostream& o, direction d)
{
    o << d.as_string();
    return (o);
}


const direction N (0);
const direction NE(1);
const direction E (2);
const direction SE(3);
const direction S (4);
const direction SW(5);
const direction W (6);
const direction NW(7);
const direction NO_DIRECTION(8);
#ifdef direction_test
#include "stream.h"
main()
{
    direction d(0);

    for(int i =0 ; i < 10; i++)
    {
	cout << d << ' ' << d.right() << ' ' << d.back() << ' ' << d.left() << '\n';
	d++;
    }
}
#endif

