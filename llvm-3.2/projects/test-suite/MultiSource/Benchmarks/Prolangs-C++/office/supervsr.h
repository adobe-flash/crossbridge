                                      // Chapter 11 - Program 3
#ifndef SUPERVSR_H
#define SUPERVSR_H

// This defines three subclasses of the parent type person. Different
//  data is stored for the different job classifications to illustrate
//  that it can be done.

#include "person.h"

class supervisor : public person {
   char *title;
public:
   supervisor(char *in_name, int in_salary, char *in_title);
   void display(void);
};



class programmer : public person {
   char *title;
   char *language;
public:
   programmer(char *in_name, int in_salary, char *in_title,
                  char *in_language);
   void display(void);
};



class secretary : public person {
   char shorthand;
   int typing_speed;
public:
   secretary(char *in_name, int in_salary,
                  int in_shorthand, int in_typing_speed);
   void display(void);
};

class consultant : public person {
   char *specialty;
   int contract_length;
public:
   consultant(char *in_name, int in_salary,
                  char *in_specialty, int in_contract_length);
   void display(void);
};

#endif
