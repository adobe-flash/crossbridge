                                   // Chapter 11 - Program 1
#ifndef PERSON_H
#define PERSON_H

class person {
protected:       // Make these variables available to the subclasses
   char *name;
   int salary;
public:
   person(void) {}
   virtual void display(void) = 0;
};

#endif
