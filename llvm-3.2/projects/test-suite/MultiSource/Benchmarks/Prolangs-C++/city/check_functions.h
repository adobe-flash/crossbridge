// check_functions.h


#ifndef _check_function
#define _check_function

#include "direction.h"
#include "roadlet.h"
#include "vehicle.h"
#include "stream.h"

extern roadlet *return_null(roadlet*, vehicle *, direction);
extern roadlet *is_empty(roadlet *, vehicle *, direction);
extern roadlet *lane_switch_ok(roadlet *, vehicle *, direction);
extern roadlet *strait(roadlet *, vehicle *, direction);
extern roadlet *strait_or_left(roadlet *, vehicle *, direction);
extern roadlet *strait_or_right(roadlet *, vehicle *, direction);
extern roadlet *green_light(intersection_roadlet*, vehicle*, direction);
extern roadlet *green_OR_plan_rightONred(intersection_roadlet*, vehicle*, direction);

#endif 
