
/****
    Copyright (C) 1996 McGill University.
    Copyright (C) 1996 McCAT System Group.
    Copyright (C) 1996 ACAPS Benchmark Administrator
                       benadmin@acaps.cs.mcgill.ca

    This program is free software; you can redistribute it and/or modify
    it provided this copyright notice is maintained.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
****/

#include <stdio.h>
#include "struktur.h"
#include "headers.h"

/* ********************************************************************* */
/* FUNCTION: midpoint - returns the midpoint of the points p1 and p2     */

dpoint midpoint(point p1, point p2) {
  dpoint p;

  p.x=((double)p1.x+(double)p2.x)/2.0;
  p.y=((double)p1.y+(double)p2.y)/2.0;

  return p;
}

/* ********************************************************************* */
/* FUNCTION: vector - returns the linesegment p1p2                       */

point vector(point p1, point p2) {
  point p;

  p.x=p2.x-p1.x;
  p.y=p2.y-p1.y;

  return p;
}

/* ********************************************************************* */
/* FUNCTION: length2 - returns the square of the length of vector p1p2   */

int length2(point p1, point p2) {
  return ((p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y));
}

/* ********************************************************************* */
/* FUNCTION: calculate_c - calculates the constant c in the equation:
             ax+by=c                                                     */

double calculate_c(point normalvector,dpoint midpoint) {
  return (double)normalvector.x*midpoint.x+
    (double)normalvector.y*midpoint.y;
}

/* ********************************************************************* */
/* FUNCTION: intersect - returns the intersection between the linesegments
             decided by n1, c1 and n2, c2                                */

dpoint intersect(point n1, point n2, double c1, double c2) {
  dpoint p;

  p.x= (c1*(double)n2.y-(double)n1.y*c2)/((double)n1.x*(double)n2.y-
					  (double)n1.y*(double)n2.x);
  p.y= ((double)n1.x*c2-c1*(double)n2.x)/((double)n1.x*(double)n2.y-
					  (double)n1.y*(double)n2.x);

  return p;
}

/* ********************************************************************* */
/* FUNCTION: centre - returns the centre of the circle through points
             p1, p2 and p3                                               */

dpoint centre(point p1, point p2, point p3) {
  point n1,n2;
  double c1,c2;

  n1=vector(p1,p2);
  n2=vector(p2,p3);
  c1=calculate_c(n1,midpoint(p1,p2));
  c2=calculate_c(n2,midpoint(p2,p3));
  return intersect(n1,n2,c1,c2);
}		   

/* ********************************************************************* */
/* FUNCTION: radius2 - finds the squared distance from p1 to c           */

double radius2(point p, dpoint centre) {
  return (((double)p.x-centre.x)*((double)p.x-centre.x)+
	  ((double)p.y-centre.y)*((double)p.y-centre.y));
}
