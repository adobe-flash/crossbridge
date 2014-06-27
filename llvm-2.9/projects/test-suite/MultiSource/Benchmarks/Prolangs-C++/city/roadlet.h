// roadlet.h

#ifndef _roadlet_h
#define _roadlet_h

#include "sim.h"
#include "light.h"
extern roadlet *return_null();
#include "check_functions.h"
#include "broken_light.h"

typedef roadlet * (*move_function)(roadlet *, vehicle *, direction);


class roadlet
{
private:
    void init(const char *n);

protected:
    const char *name;
    vehicle *occupant;
    roadlet *neighbors[8];
    move_function the_moves[8];

public:
    roadlet() {
      init("unnamed");
    };
    roadlet(const char *n)   {
      init(n);
    };
    int occupied()           { return(occupiedby() != NULL);};
    vehicle *occupiedby()    {return(occupant);};
    void arrive(vehicle *v)  {occupant = v;};
    vehicle *depart()	     {vehicle *v=occupant; occupant=NULL; return(v);};

    void set_neighbor(direction d, roadlet *n) { neighbors[d.as_int()] = n;};
    roadlet *neighbor(direction d)            { return(neighbors[d.as_int()]);};

    move_function *moves()  { return(the_moves);};
    void set_move_filter_function(direction d, move_function f)
	{the_moves[d.as_int()] = f; };

    friend ostream& operator<< (ostream&, roadlet);
};


class intersection_roadlet : public roadlet
{
protected:
    light *l;

public:
    intersection_roadlet(const char *name, light *alight) : roadlet(name) {l = alight;} ;
    light *get_light() { return(l);};
};

#endif
