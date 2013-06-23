#include "port.h"
/*
Types for fast 2D Manhattan transformation package.

Copyright Ken Keller 1981
*/

#define TSTKSIZE 100

typedef struct MT {
  /*Transformation stack pointer.*/
  int sp;
  /*Transformation stack.*/
  int stk[TSTKSIZE][3][3];
  /*Current transform.*/
  int t[3][3];
  /*Inverse of current transform.*/
  int ti[3][3]; }
  MT ;

extern MT *MTBegin();
