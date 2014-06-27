
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
#include <stdlib.h>
#include "struktur.h"
#include "headers.h"
extern int default_radius;
extern int CHno;

/* ********************************************************************* */
/* FUNCTION: create_point - creats a point for the circular list 
             containing the points on the convex hull                    */

CHpoints *create_point(point p) {
  CHpoints *new_point;
  if (!(new_point = (CHpoints *)malloc(sizeof(CHpoints)))) {
    printf("Can't create point\n");
    exit(0);
  }
  new_point->prev = NULL;
  new_point->next = NULL;
  new_point->node = p;
  new_point->number=CHno;
  return new_point;
}

/* ********************************************************************* */
/* PROCEDURE: point_list_insert - inserts a new point in the circular 
              list.                                                     */
    
void point_list_insert(CHpoints **PL, point p) {
  CHpoints *temp;
  
  CHno++;                       /* increase the number of CHnodes by one */  
  temp=create_point(p);
  if ((*PL)==NULL) {
    temp->prev = temp;
    temp->next = temp;
    *PL=temp; }
  else {
    temp->next = *PL;
    (*PL)->prev->next = temp;
    temp->prev = (*PL)->prev;
    (*PL)->prev = temp;
    (*PL)=temp; }
  
}

/* ********************************************************************* */
/* FUNCTION: before - returns the node in the circular list previous
             of p (counterclockwise)                                     */

CHpoints *before(CHpoints *P) {
  return (P->prev);
}

/* ********************************************************************* */
/* FUNCTION: before - returns the node in the circular list next
             to p (clockwise)                                            */

CHpoints *next(CHpoints *P) {
  return (P->next);
}

/* ********************************************************************* */
/* FUNCTION: angle - computes the semi angle between the vectors p2p1
             and p2p3                                                    */

double angle(CHpoints *p1, CHpoints *p2, CHpoints *p3) {
  double dot_product,la2,lb2,v;
  point a,b;
  
  a=vector(p2->node,p1->node);
  b=vector(p2->node,p3->node);
  dot_product=(a.x*b.x+a.y*b.y);
  la2=(a.x*a.x+a.y*a.y);
  lb2=(b.x*b.x+b.y*b.y);
  v=(dot_product*dot_product)/(la2*lb2);
  if (dot_product<0)
    return -v;
  else
    return v;
}

/* ********************************************************************* */
/* PROCEDURE: number_points - numbers the points                         */

void number_points(CHpoints *PL) {
  CHpoints *temp;
  char c[5];

  temp = PL->next;
  while (temp != PL) 
    temp=temp->next; 
}

/* ********************************************************************* */
/* FUNCTION: remove_points - removes points in the circular list to avoid
             that any 3 consecutive points form a straight line          */

CHpoints *remove_points(CHpoints *PL) {
  CHpoints *temp,*rem;
  point a,c,b; 
  int i=0;
  bool done;
  done=false;

  temp = PL->next;
  
  while ((temp != PL) && (temp->next != temp)) {
    a=temp->prev->node;
    b=temp->node;
    c=temp->next->node;
    
    if (((b.y-c.y)*(b.x-a.x)+(c.x-b.x)*(b.y-a.y) == 0) &&
       (((a.x < b.x) && (b.x < c.x)) || 
	((a.x > b.x) && (b.x > c.x)) ||
	((a.x == b.x) && (a.x == c.x) && (a.y < b.y) && (b.y < c.y)) ||
	((a.x == b.x) && (a.x == c.x) && (a.y > b.y) && (b.y > c.y)))) {
      rem = temp;
      temp->next->prev = temp->prev;
      temp->prev->next = temp->next;
      temp=temp->next;
      free(rem);  
      CHno--;                   /* Decrease the number of CHnodes by one */
    }
    else
      temp=temp->next;
  }
  a=temp->prev->node;
  b=temp->node;
  c=temp->next->node;

  if (((b.y-c.y)*(b.x-a.x)+(c.x-b.x)*(b.y-a.y) == 0) &&
       (((a.x < b.x) && (b.x < c.x)) || 
	((a.x > b.x) && (b.x > c.x)) ||
	((a.x == b.x) && (a.x == c.x) && (a.y < b.y) && (b.y < c.y)) ||
	((a.x == b.x) && (a.x == c.x) && (a.y > b.y) && (b.y > c.y)))) {   
    rem = temp;
    temp->next->prev = temp->prev;
    temp->prev->next = temp->next;
    temp=temp->next;
    free(rem);
    CHno--;                   /* Decrease the number of CHnodes by one */
  }
  return temp;
}
