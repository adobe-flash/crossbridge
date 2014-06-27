// vehicle.cc
// speed in (ticks per roadlet) / 100
// speed < 100 yields multiple ticks per move.
// speed > 100 yields multiple moves per tick.

#include <stream.h>
#include <stdio.h>
#include "sim.h"
#include "direction.h"
#include "vehicle.h"
#include "roadlet.h"


ostream& operator<<(ostream& o, vehicle v)
{
    o << v.name << " at " << *(v.location) << " going " << v.get_direction();
    return (o);
}


void vehicle::tick()
{
    move_points = move_points + 100;
    while (move_points >= speed)
    {
	move();
	move_points = move_points - speed;
    }
}


void vehicle::move()
{
    direction d = NO_DIRECTION;

    if (plan != -1)
    {
	cout << "there is a plan! "<< plan << " \n";
        if (location->moves()[plan](location->neighbor(plan),this,plan) != NULL)
	{
	    d = direction(plan);
	    plan = -1;
	}  // else wait for plan to be possible
    }
    else
        d = select_move();

    if (d != NO_DIRECTION)
    {
        roadlet *r;

        location->depart();
        r = location->neighbor(d);
        r->arrive(this);
        location = r;
	if ((d == N) || (d == S) || (d == E) || (d == W))
	    dir = d;	// else = lane change only

    }
}


direction vehicle::select_move()
{
    roadlet *(**possible_moves)(roadlet*, vehicle *, type_direction);
    direction dir, move_to_the[8];
    int used;

    possible_moves = location->moves();

    for(used=0, dir=N; dir<=NW; ++dir)
    {
	// cout << "possible_moves[" << dir << "] " << possible_moves[dir.as_int()] << '\n';
	if (possible_moves[dir.as_int()](location->neighbor(dir), this, dir) != NULL)
	{
	    move_to_the[used] = dir;
	    used++;
	}
    }

    if (used == 0)
    {
        //cout << "vehicle::select_move "<< *this << " has no where to go!\n";
	return(NO_DIRECTION);
    }
    else
    {
        int use = (int)random() % used;
	return(move_to_the[use]);
    }
}


// Car stuff
ostream& operator<< (ostream& o, car c)
{
    o << "Car " << (vehicle)c; 
    return (o);
}


// Truck stuff
ostream& operator<< (ostream& o, truck t)
{
    o << "Truck " << (vehicle)t; 
    return (o);
}
