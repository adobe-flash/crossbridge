// build_intersection.cc

#include "intersection.h"
#include <cstdlib>
#include <cstring>

void intersection_4x4::connectNin (roadlet *leftlane, roadlet *rightlane)
{
    connect(leftlane, S, roadlets[0][1], N, (filter_fn)green_light);
    connect(rightlane, S, roadlets[0][0], N, (filter_fn)green_OR_plan_rightONred);
}

void intersection_4x4::connectEin (roadlet *leftlane, roadlet *rightlane)
{
    connect(leftlane, W, roadlets[1][3], E, (filter_fn)green_light);
    connect(rightlane, W, roadlets[0][3], E, (filter_fn)green_OR_plan_rightONred);
}

void intersection_4x4::connectSin (roadlet *leftlane, roadlet *rightlane)
{
    connect(leftlane, N, roadlets[3][2], S, (filter_fn)green_light);
    connect(rightlane, N, roadlets[3][3], S, (filter_fn)green_OR_plan_rightONred);
}

void intersection_4x4::connectWin (roadlet *leftlane, roadlet *rightlane)
{
    connect(leftlane, E, roadlets[2][0], W, (filter_fn)green_light);
    connect(rightlane, E, roadlets[3][0], W, (filter_fn)green_OR_plan_rightONred);
}

void intersection_4x4::connectNout(roadlet *leftlane, roadlet *rightlane)
{
    connect(roadlets[0][2], N, leftlane, S,(filter_fn) strait);
    connect(roadlets[0][3], N, rightlane, S, (filter_fn)strait_or_right);
}
void intersection_4x4::connectEout(roadlet *leftlane, roadlet *rightlane)
{
    connect(roadlets[2][3], E, leftlane, W, (filter_fn)strait);
    connect(roadlets[3][3], E, rightlane, W, (filter_fn)strait_or_right);
}
void intersection_4x4::connectSout(roadlet *leftlane, roadlet *rightlane)
{
    connect(roadlets[3][1], S, leftlane, N, (filter_fn)strait);
    connect(roadlets[3][0], S, rightlane, N, (filter_fn)strait_or_right);
}
void intersection_4x4::connectWout(roadlet *leftlane, roadlet *rightlane)
{
    connect(roadlets[1][0], W, leftlane, E, (filter_fn)strait);
    connect(roadlets[0][0], W, rightlane, E, (filter_fn)strait_or_right);
}


intersection_4x4::intersection_4x4(const char *name)
{
    char *buff;

    int i,j;

    light_type *l;

    l = new light();
    the_light = l;

    for(i=0; i<4; i++)
	for(j=0; j<4; j++)
	{
	    buff = (char*)malloc(strlen(name) + 7);
	    sprintf(buff, "%s %d %d", name, i, j);
	    roadlets[i][j] = new intersection_roadlet(buff, l);
	}


    for(i=3; i>0; i--)
    {
	connect(roadlets[i][3], N, roadlets[i-1][3], S, &strait);
	connect(roadlets[i][2], N, roadlets[i-1][2], S, &strait);
    }
    for(i=0; i<3; i++)
    {
	connect(roadlets[i][0], S, roadlets[i+1][0], N, &strait);
	connect(roadlets[i][1], S, roadlets[i+1][1], N, &strait);
    }
    for(j=0; j<3; j++)
    {
	connect(roadlets[2][j], E, roadlets[2][j+1], W, &strait);
	connect(roadlets[3][j], E, roadlets[3][j+1], W, &strait);
    }
    for(j=3; j>0; j--)
    {
	connect(roadlets[0][j], W, roadlets[0][j-1], E, &strait);
	connect(roadlets[1][j], W, roadlets[1][j-1], E, &strait);
    }

    // override center square
// FIX ME i think car can circle forever in the middle of the intersection!
// FIX with PLAN of lefty is strait
    connect(roadlets[2][2], N, roadlets[1][2], S, &strait_or_left);
    connect(roadlets[1][2], W, roadlets[1][1], E, &strait_or_left);
    connect(roadlets[1][1], S, roadlets[2][1], N, &strait_or_left);
    connect(roadlets[2][1], E, roadlets[2][2], W, &strait_or_left);

car b("blocker");
b.set_location(roadlets[0][2]);         // a car
roadlets[0][2]->arrive(&b);
cout << b << '\n';


}
