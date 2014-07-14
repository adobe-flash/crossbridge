// vehicle.h

#ifndef _vehicle_h
#define _vehicle_h

#include "sim.h" 

class vehicle
{
private:
    enum { DEFAULT_SPEED = 100 };
    roadlet *location;
    const char *name;
    int speed;
    int move_points;			// move units accumulated towards moving
    void init(roadlet *l, const char *n, int s) 
	{plan=-1,location=l, name=n, speed=s, move_points=0, dir = N;};

protected:
    direction dir;
    direction select_move();
    void move();

public:
    vehicle() {init(NULL, NULL, DEFAULT_SPEED);};
    vehicle(const char *n) {init(NULL, n, DEFAULT_SPEED);};
    vehicle(roadlet *r, char *n) {init(r, n, DEFAULT_SPEED);};
    vehicle(roadlet *r, char *n, int s) {init(r, n, s);};
    // others include: r, s, rs, sr, ....

    int plan;

    direction get_direction() {return (dir);};
    void set_location(roadlet *r)  {location = r;};

    void tick();

    friend ostream& operator<< (ostream&, vehicle);
};

class car : public vehicle
{
public:
    car() : vehicle() { };
    car(const char *n) : vehicle(n) { };
    friend ostream& operator<< (ostream&, car);
};

class truck : public vehicle
{
protected:
    int weight;

public:
    friend ostream& operator<< (ostream&, truck);
    truck() { } ; // fixes moves array to be "if over weight then no else old function "
};
#endif
