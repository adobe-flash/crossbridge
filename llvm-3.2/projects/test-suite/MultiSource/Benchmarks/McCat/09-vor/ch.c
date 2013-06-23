
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

extern splay_node *Splaytree;
extern int CHno;

/* ********************************************************************* */
/* FUNCTION: empty - determins if the tree-structure is empty            */

bool empty() {
  return (Splaytree == NULL);
}

/* ********************************************************************* */
/* FUNCTION: point_equal - determins if the two points of the argument
   are the same                                                          */

bool point_equal(point p1,point p2) {
  return  ((p1.x == p2.x) && (p1.y == p2.y)) ;
}

/* ********************************************************************* */
/* FUNCTION: determinant - computes the determinant
   | p1.x p1.y 1 |
   | p2.x p2.y 1 |
   | p3.x p3.y 1 |
   which determins wether a point is left, right or on the line having
   including the segment p1p2                                            */

int determinant(point p1, point p2, point p3) {
  return (p1.x*(p2.y-p3.y)-p1.y*(p2.x-p3.x)+p2.x*p3.y-p2.y*p3.x);
}

/* ********************************************************************* */
/* FUNCTION: visible - determins if the point p3 can be reached from p2 
   with a straight line, in other words, is the point p3 left/right of the 
   segment p1->p2                                                        */

bool visible(int direction,point p1,point p2,point p3) {
  if (direction==l) 
    return (determinant(p1,p2,p3) < 0);
  else
    return (determinant(p1,p2,p3) > 0);  
}

/* ********************************************************************* */
/* FUNCTION: get_points_on_hull - gets the points on the convex hull and
             inserts them into a linked list                             */

CHpoints *get_points_on_hull(DCEL_segment *left,DCEL_segment *right) {
  DCEL_segment *n;
  DCEL_segment *end, *tmp;
  CHpoints *P=NULL;

  end = right;

  /*  line((int) right->v1.x,(int) right->v1.y,
       (int) right->v2.x,(int) right->v2.y); */
  point_list_insert(&P,left->v2);
  
  if (left->q1 == right)
    tmp=left->p2;
  else
    tmp=left->p1;
  right=left;
  left=tmp;

  if (left != end) {
    while (left != end) {
      /* line((int) right->v1.x,(int) right->v1.y,
	   (int) right->v2.x,(int) right->v2.y); */
      if (left->q1 == right) {
	point_list_insert(&P,left->v1);
	tmp=left->p2; }
      else {
	point_list_insert(&P,left->v2);
	tmp=left->p1; }
      right=left;
      left=tmp; } 
    if (left->q1 == right) 
      point_list_insert(&P,left->v1);
    else 
      point_list_insert(&P,left->v2);
    /* line((int) right->v1.x,(int) right->v1.y,
	 (int) right->v2.x,(int) right->v2.y);*/ }
  else {
    if (!(point_equal(right->v2,right->v1)))
      point_list_insert(&P,right->v1); }

  return P;
}

/* ********************************************************************* */
/* PROCEDURE: add_segments - connects n->v2 with all visible points left
   or right of the segment n                                             */

void add_segments(DCEL_segment *n,DCEL_segment *act,DCEL_segment *first,
		  int direction) {
  DCEL_segment *k; /* the new segment */
  point p;

  /* p will contain the point of the segment act that doesn't touch n */
  if (point_equal(act->v1,n->v1)) 
    p = act->v2;
  else 
    p = act->v1;
  
  if (visible(direction,n->v1,n->v2,p)) {
    /* p is visible from n.v2 */
    k = (DCEL_segment *) malloc (sizeof(DCEL_segment));
    
    /* set the coordinats of the new segment */
    k->v1 = p;       /* The "smallest" point */
    k->v2 = n->v2;

    if (direction == l) { 
      /* Add segments left of the new line */
      k->p1 = act->p1;                                /* no 5. */
      k->p2 = n;                                      /* no 2. */
      k->q1 = act;                                    /* no 4. */
      k->q2 = first;                                  /* no 8. */

      n->q2 = k;                                      /* no 1. */
      
      /* set_previous_segment_pointer                    no 7. */
      if (point_equal(act->p1->v1,p))
	act->p1->q1 = k;
      else
	act->p1->q2 = k;
      
      act->p1 = k;                                    /* no 6. */
      first->p2 = k;                                  /* no 3. */ 
      add_segments(k,k->p1,first,direction); }
    else {
      /* Add segments right of the new line */
      k->q1 = act->q1;                                /* no 5. */
      k->q2 = n;                                      /* no 8. */
      k->p1 = act;                                    /* no 4. */
      k->p2 = first;                                  /* no 2. */
      
      n->p2 = k;                                      /* no 1. */
      
      /* set_previous_segment_pointer                    no 7. */
      if (point_equal(act->q1->v1,p))
	act->q1->p1 = k;
      else
	act->q1->p2 = k;
      
      act->q1 = k;                                    /* no 6. */
      first->q2 = k;                                  /* no 3. */ 
      add_segments(k,k->q1,first,direction); }
  }
}

/* ********************************************************************* */
/* FUNCTION: construct_ch - finds the points on the convex hull and
             drops the rest                                              */

CHpoints *construct_ch() {
  DCEL_segment *root;
  /* Left and right are the pointer that point to the segments to
     the left and right of the rightmost point in the convex hull
     at a given time. */
  struct DCEL_segment *left,*right;
  
  CHno=0;   /* reset number of points on hull */
  
  if (!empty()) { 
    /* At least one point was plotted */
    
    /* First segment */
    root=(DCEL_segment *) malloc(sizeof(DCEL_segment));
    root->v1 = delete_min(&Splaytree);
    root->p1 = root;
    root->p2 = root;
    root->q1 = root;
    root->q2 = root;
 
    /* Setup left and right initally */
    left=root;
    right=root;
    
    if (empty()) {
      /* only one point was plotted */
      root->v2 = root->v1; }
    else { 
      /* more than one point was plotted */
      root->v2 = delete_min(&Splaytree);

      /* As long as there are points left extend the convex hull */
      while (!empty()) {
	DCEL_segment *n;
	n = (DCEL_segment *) malloc (sizeof(DCEL_segment));
	
	/* Construct new segment */
	n->v1 = left->v2;
	n->v2 = delete_min(&Splaytree);
	
	/* Set the new segment's pointers */
	n->p1 = left;
	n->q1 = right;
	n->p2 = n;
	n->q2 = n;
	
	/* Set pointers in left and right to point to n. These
	   pointers are the * marked ones in the figure 2 */
	right->p2 = n;
	left->q2 = n;
	
	/* Connect all the visible points "left" of the segment */
	add_segments(n,left,n,l);
	left = n->p2;
	
	/* Connect all the visible points "right" of the segment */
	add_segments(n,right,left,r);
	right = left->q2; }
    }
    free_tree(Splaytree);
    return remove_points(get_points_on_hull(left,right));
  }
}
