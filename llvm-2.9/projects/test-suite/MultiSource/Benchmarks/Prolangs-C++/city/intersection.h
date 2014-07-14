// build_intersection.cc

#ifndef _intersection_h
#define _intersection_h

#include "stdio.h"
#include "direction.h"
#include "roadlet.h"
#include "check_functions.h"
#include "light.h"

// this function is not very OO. can that be fixed?


extern void connect(roadlet *, direction, 
	            roadlet *, direction, 
	            roadlet * (*r1_to_r2_filter_function)(roadlet *, vehicle *, direction));

typedef roadlet * (*filter_fn)(roadlet *, vehicle *, direction);

class intersection
{
protected:
    light *the_light;
public:
    light *get_light() {return (the_light);};
};

class intersection_2x2 : public intersection
{
};

class intersection_4x4 : public intersection
{
protected:
    roadlet *roadlets[4][4];

public:
    intersection_4x4(const char *name);
    void connectSin(roadlet*, roadlet*) ;
    void connectNout(roadlet*, roadlet*);
    void connectNin(roadlet*, roadlet*);
    void connectSout(roadlet*, roadlet*);
    void connectEin(roadlet*, roadlet*);
    void connectWout(roadlet*, roadlet*);
    void connectWin(roadlet*, roadlet*);
    void connectEout(roadlet*, roadlet*);
};

#endif
