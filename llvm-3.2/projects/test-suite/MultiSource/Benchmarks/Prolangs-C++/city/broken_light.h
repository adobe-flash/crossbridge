// broken_light.cc

#ifndef _broken_light_h
#define _broken_light_h

#include "light.h"
#include "stream.h"
#include <cstdlib>

#define BROKEN_LIGHT_ID 1

class broken_light : public light
{
public:
    virtual int next_state();
    broken_light() : light() { __ = BROKEN_LIGHT_ID; };
    broken_light(int t1, int t2, int t3, int t4) : light (t1,t2,t3,t4){};
};

inline int broken_light::next_state()
{
    //cout << "next state called\n";
    if ((random() % 4) == 1)
	return (current_state);
    else
        return ((current_state + 1 ) % 4);
}

#endif
