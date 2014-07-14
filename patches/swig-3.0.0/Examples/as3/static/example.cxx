/* File : example.c */

#include "example.h"
#define M_PI 3.14159265358979323846

double Circle::area(void) {
  return M_PI*radius*radius;
}

double Circle::perimeter(void) {
  return 2*M_PI*radius;
}

double Circle::getPi(void) {
  return M_PI;
}

