// direction.h

#ifndef _direction_h
#define _direction_h

#include <stream.h>

class direction
{
private:
    int dir;

public:
    direction()     	{dir = 0;};
    direction(int i) 	{dir = i % 9;};

    int as_int()	{return (dir);};
    const char *as_string();
    direction right_front()	{return(direction((dir + 1) % 8));};
    direction right()		{return(direction((dir + 2) % 8));};
    direction right_back()	{return(direction((dir + 3) % 8));};
    direction back()		{return(direction((dir + 4) % 8));};
    direction left_back() 	{return(direction((dir + 5) % 8));};
    direction left()		{return(direction((dir + 6) % 8));};
    direction left_front()	{return(direction((dir + 7) % 8));};

    friend ostream& operator<< (ostream&, direction);
    friend int operator<= (direction d1, direction d2);
    friend int operator== (direction d1, direction d2);
    friend int operator!= (direction d1, direction d2);
    direction operator++ () {dir++; return(*this);}; // both pre and post
};


extern const direction N ;
extern const direction NE;
extern const direction E ;
extern const direction SE;
extern const direction S ;
extern const direction SW;
extern const direction W ;
extern const direction NW;
extern const direction NO_DIRECTION;



typedef direction type_direction;
#endif
