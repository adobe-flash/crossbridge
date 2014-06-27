// main.c   -   simulation main driver

#include "stream.h"
#include "sim.h"
#include "direction.h"
#include "vehicle.h"
#include "roadlet.h"
#include "check_functions.h"
#include "intersection.h"
#include <cstdlib>


// connect r1->d1 to r2->d2
void connect(roadlet *r1, direction d1, 
	     roadlet *r2, direction d2, 
	     roadlet* (*r1_to_r2_filter_function)(roadlet *, vehicle *, direction))
{
    //cout << "connecting " << *r1 << " " << *r2 << '\n';
    r1->set_neighbor(d1, r2);
    r2->set_neighbor(d2, r1);
    r1->set_move_filter_function(d1, r1_to_r2_filter_function);
}

int main()
{
    car c("fred");
    roadlet *r1, *r2;
    roadlet *r3, *r4;
    char *buff;
    int i;

    srandom(5);

    // build a two lane one direction road

    r1 = new roadlet ("R start");
    r3 = new roadlet ("L start");
    connect(r1, W, r3, E, return_null); // there but can't move to

    c.set_location(r1);		// a car
    r1->arrive(&c);

    for(i=0; i<10; i++)
    {
	buff = (char *)malloc(4);
	sprintf(buff, "R%d", i);
	r2 = new roadlet(buff);

	buff = (char *)malloc(4);
	sprintf(buff, "L%d", i);
	r4 = new roadlet(buff);

        connect(r1, N, r2, S, &is_empty);
        connect(r3, N, r4, S, &is_empty);
        connect(r1, NW, r4, SE, &lane_switch_ok);
        connect(r3, NE, r2, SW, &lane_switch_ok);
        connect(r2, W, r4, E, return_null); // can't more sideways

	r1 = r2;
	r3 = r4;
    }

    car b2("blocker 2");
    b2.set_location(r1);		// a car
    r1->arrive(&b2);
    cout << b2 << '\n';

    intersection_4x4 i1("intersection ");

    i1.connectSin(r3, r1);

    broken_light l(3,1,4,1);

    for(i = 0; i < 100000; i++)
    {
	//cout << l << "\n";
	l.tick();
    }

    for(i=0; i< 100000; i++)
    {
        i1.get_light()->tick();
    }


    r1 = new roadlet ("East Road R ");
    r3 = new roadlet ("East Road L ");
    connect(r1, N, r3, S, return_null); // there but can't move to

    car b("blocker");
    b.set_location(r1);		// a car
    r1->arrive(&b);
    cout << b << '\n';

    i1.connectEout(r3, r1);

    for(i=0; i< 100000; i++)
    {
        //cout << '\n' << i << ' ' << c << '\n';
        c.tick();
    }
    //cout << i << ' ' << c << '\n';
}
