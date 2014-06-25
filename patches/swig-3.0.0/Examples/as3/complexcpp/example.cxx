/* File : example.cxx */

#include <stdio.h>
#include "example.h"

// PRINT POINTER

void printPointer(struct Bar *number){
    printf("printPointer: %d\n", number->num);
}

// SUPPRESS OVERRIDE

class base { 
protected: 
virtual ~base(){} 
}; 

class derived : public base { 
public: 
virtual ~derived() {} 
}; 

// MULTI TYPEMAP

void print_str(int len, char *str) {
  char buf[len + 1];
  snprintf(buf, len + 1, "%s", str);
  printf("%s\n", buf);
} 

// COPY CONSTRUCTOR

void CRectangle::set_values (int a, int b) {
    x = a;
    y = b;
}

// constructor
CRectangle::CRectangle(){
    x = 2;
    y = 2;
}

// copy constructor
CRectangle::CRectangle(const CRectangle& c){
    printf("in copy constructor\n"); // output of this test should include this string
    x = c.x;
    y = c.y;
}

int createRectAndGetArea() {
    // call the normal constructor
    CRectangle rect1;
    rect1.set_values(3, 4);
    
    // call the copy constructor
    CRectangle rect2 = rect1;
    
    // return the area (should be 3*4=12)
    return rect2.area();
}

// AS3 IMPORT

void printNumber(int i){
    printf("printNumber: 4==%d\n", i);
}

void printNumber2(int i){
    printf("printNumber2: 4==%d\n", i);
} 

// SDTINT
/* Some global variables */
int32_t a;
int_least16_t b;
int_fast8_t c;
uint8_t d;
intptr_t e;

 
// NESTED CPP
/*
int MotorCar::DesignOpinion::AceDesignCount = 0;
int MotorCar::DesignOpinion::TotalDesignCount = 0;

int MotorCar::DesignOpinion::PercentScore() {
  return AceDesignCount*100/TotalDesignCount;
}

MotorCar::Wheels::Wheels(Shape shape, size_t count) : shape(shape), count(count) {}

MotorCar::WindScreen::WindScreen(bool opaque) : opaque(opaque) {}

MotorCar::MotorCar(const std::string &name, const Wheels &wheels, const WindScreen &windscreen) : name(name), wheels(wheels), windscreen(windscreen) {}

MotorCar MotorCar::DesignFromComponents(const std::string &name, const Wheels &wheels, const WindScreen &windscreen) {
  MotorCar car = MotorCar(name, wheels, windscreen);
  DesignOpinion::TotalDesignCount++;
  if (car.wheels.Opinion().itrocks && car.windscreen.Opinion().itrocks)
    DesignOpinion::AceDesignCount++;
  return car;
}

MotorCar::DesignOpinion MotorCar::Wheels::Opinion() {
  DesignOpinion opinion;
  opinion.itrocks = true;
  if (shape == Square) {
    opinion.itrocks = false;
    opinion.reason = "you'll have a few issues with wheel rotation";
  }
  if (count <= 2) {
    opinion.reason += opinion.itrocks ? "" : " and ";
    opinion.itrocks = false;
    opinion.reason += "a few more wheels are needed for stability";
  }
  if (opinion.itrocks)
    opinion.reason = "your choice of wheels was top notch";

  return opinion;
}

MotorCar::DesignOpinion MotorCar::WindScreen::Opinion() {
  DesignOpinion opinion;
  opinion.itrocks = !opaque;
  opinion.reason = opinion.itrocks ? "the driver will have a commanding view out the window" : "you can't see out the windscreen";
  return opinion;
}

std::string MotorCar::WillItWork() {
  DesignOpinion wh = wheels.Opinion();
  DesignOpinion ws = windscreen.Opinion();
  std::string willit;
  if (wh.itrocks && ws.itrocks) {
    willit = "Great car design because " + wh.reason + " and " + ws.reason;
  } else {
    willit = "You need a rethink because ";
    willit += wh.itrocks ? "" : wh.reason;
    willit += (!wh.itrocks && !ws.itrocks) ? " and " : "";
    willit += ws.itrocks ? "" : ws.reason;
  }
  return willit;
}
*/