// Listing 4.1 page 72, from "C++ Programming Style" by
// Tom Cargill.  Addison Wesley, 1992.

#include <stdio.h>
#include <string.h>

class Vehicle {
protected:
  char *plate;
public:
  Vehicle() {plate = NULL;}
  Vehicle(char *p) {
    plate = new char[strlen(p)+1];
    strcpy(plate,p);
  }
  ~Vehicle() {delete [] plate;}
  virtual void identify() {
    printf("generic vehicle\n");
  }
};

class Car : public Vehicle {
public:
  Car() : Vehicle() {}
  Car(char *p) : Vehicle(p) {}
  void identify() {
    printf("car with plate %s\n", plate);
  }
};

class Truck : public Vehicle {
public:
  Truck() : Vehicle() {}
  Truck(char *p) : Vehicle(p) {}
  void  identify() {
    printf("truck with plate %s\n", plate);
  }
};

class Garage {
  int maxVehicles;
  Vehicle **parked;
public:
  Garage(int max);
  ~Garage();
  int accept(Vehicle *);
  Vehicle *release(int bay);
  void listVehicles();
};

Garage::Garage(int max) {
  maxVehicles = max;
  parked = new Vehicle* [maxVehicles];
  for (int bay = 0; bay < maxVehicles; ++bay)
    parked[bay] = NULL;
}

Garage::~Garage() {
  delete [] parked;
}

int Garage::accept(Vehicle *veh) {
  for (int bay=0; bay < maxVehicles; ++bay)
    if (!parked[bay]) {
      parked[bay] = veh;
      return bay;
    }
  return -1; // no bay available
}
  
Vehicle *Garage::release(int bay) {
  if (bay < 0 || bay >= maxVehicles)
    return NULL;
  Vehicle *veh = parked[bay];
  parked[bay] = NULL;
  return veh;
}

void Garage::listVehicles() {
  for (int bay=0; bay < maxVehicles; ++bay)
    if (parked[bay]) {
      printf("Vehicle in bay %d is: ", bay);
      parked[bay]->identify();
    }
}

Car c1("RVR 101");
Car c2("SPT 202");
Car c3("CHP 303");
Car c4("BDY 404");
Car c5("BCH 505");

Truck t1("TBL 606");
Truck t2("IKY 707");
Truck t3("FFY 808");
Truck t4("PCS 909");
Truck t5("SLY 000");

// LLVM: add main return type.
int main () {

  Garage park(15);

  park.accept(&c1);

  int t2bay = park.accept(&t2);

  park.accept(&c3);
  park.accept(&t1);

  int c4bay = park.accept(&c4);

  park.accept(&c5);
  park.accept(&t5);

  park.release(t2bay);

  park.accept(&t4);
  park.accept(&t3);

  park.release(c4bay);

  park.accept(&c2);

  park.listVehicles();

  return 0;
}
