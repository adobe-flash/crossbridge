
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
#include <math.h>
#include <stdlib.h>
#include "struktur.h"
#include "headers.h"

splay_node *Splaytree; /* Tree-Structure */
CHsplay_node *CHSplaytree; /* Another Tree-Structure */
CHpoints *S;           /* S-structure */
int default_radius;    /* Default radius */
int CHno=0;            /* Number of points in the circular list */
Knode *K;              /* The Voroni vertices */
Enode *E;              /* The Voroni edges */
int Kcount=1;          /* Kcount-1 is the number of Vertices */
int Ecount=1;          /* Ecount-1 is the number of Edges */

/* ********************************************************************* */
/* PROCEDURE: clean_up - cleans up                                       */

void clean_up() {
  free(K);                                    /* Frees the Voronoi nodes */
  free(E);                                    /* Frees the Voronoi edges */
  Kcount=1; 
  Ecount=1;
}

/* ********************************************************************* */
/* PROCEDURE: getpoint - Add a new vertex at the point (x,y)             */

void getpoint(int x,int y) {
  point p;

  p.x = x;
  p.y = y;
  insert(&Splaytree,p);
}

/* ********************************************************************* */
/* PROCEDURE: get_file - fetches the points from file filename           */

void get_file(void) {
  int i,x,y,no_p;
  
  Splaytree=init();
  fscanf(stdin,"%d",&no_p);
  for (i=1; i<=no_p; i++) {
    fscanf(stdin,"%d %d\n",&x,&y);
    getpoint(x,y);
  }
}

/* ********************************************************************* */
/* PROCEDURE: add_point - adds a new point to the Voronoi diagram        */

void add_point(point p) {
  K[Kcount].p = p;
  K[Kcount].e1 = 0;               /* No edges make use of this point yet */
  K[Kcount].e2 = 0;
  K[Kcount].e3 = 0;
  Kcount++;
}

/* ********************************************************************* */
/* FUNCTION: compute_v - Computes the point on the bisector between p and
             next(p) at "infinity"                                       */

point compute_v(CHpoints *P) {

  /*                B
               -------------
              |             |        ^  ^
           A  |   SCREEN    |  C     |  |  -> ->
              |             |       Bn Dn  An Cn 
               -------------
                    D                                                    */
  
  point An,Bn,Cn,Dn;      /* Normalvectors to the Screen borders A,B,C,D */
  point n;         /* Normalvector to the bisector between P and Next(p) */
  double c;       /* the constant in the equation ax+by=c, where (a,b) is 
                                             normalvector to the bisetor */
  point i;
  dpoint di; 
         /* Point where bisector of P and next(P) intersects with border */

  point p,np;   
  double minX, minY, maxX, maxY; /* Max, min X and Y coordinate */

  minX=-10000.0;
  minY=-10000.0;
  maxX=10000.0;
  maxY=10000.0;
  An.x=1; An.y=0;           /* Setup normalvectores for the border lines */
  Bn.x=0; Bn.y=1;
  Cn.x=1; Cn.y=0;
  Dn.x=0; Dn.y=1;
  
  p=P->node;                                       /* p=P and np=next(P) */
  np=next(P)->node;
  
  n=vector(p,np);
  c=calculate_c(n,midpoint(p,np));

  if ((p.x < np.x) && (p.y < np.y)) {        /* intersects either D or A */
    di=intersect(n,Dn,c,maxY);
    if ((minX>di.x) || (di.x>maxX))
      di=intersect(n,An,c,minX);
  } 
  else if ((p.x < np.x) && (p.y == np.y)) {              /* intersects D */
    di=intersect(n,Dn,c,maxY);
  }
  else if ((p.x < np.x) && (p.y > np.y)) {   /* intersects either D or C */
    di=intersect(n,Dn,c,maxY);
    if ((minX>di.x) || (di.x>maxX))
      di=intersect(n,Cn,c,maxX);
  }
  else if ((p.x == np.x) && (p.y < np.y)) {              /* intersects A */
    di=intersect(n,An,c,minX);
  }
  else if ((p.x == np.x) && (p.y > np.y)) {              /* intersects C */
    di=intersect(n,Cn,c,maxX);
  }
  else if ((p.x > np.x) && (p.y < np.y)) {   /* intersects either A or B */
    di=intersect(n,Bn,c,minY);
    if ((minX>di.x) || (di.x>maxX))
      di=intersect(n,An,c,minX);
  }
  else if ((p.x > np.x) && (p.y == np.y))                /* intersects B */
    di=intersect(n,Bn,c,minY);
  else if ((p.x > np.x) && (p.y > np.y)) {   /* intersects either C or B */
    di=intersect(n,Bn,c,minY);
    if ((minX>di.x) || (di.x>maxX))
      di=intersect(n,Cn,c,maxX);
  }
  else {
    printf("Error: Can't intersect\n");
    exit(1);
  }
  i.x= (int)di.x;
  i.y= (int)di.y;
  return i;
}

/* ********************************************************************* */
/* PROCEDURE: add_infinit_points_to_K - initializes the v point in all
              CHpoints and inserts these in the Vornoi diagram           */

void add_infinit_points_to_K(CHpoints *S) {
  CHpoints *temp;
  key key;

  /* Allocate space for the Voronoi nodes and edges */
  K = (Knode *) calloc(2*CHno-1,sizeof(Knode));
  E = (Enode *) calloc(2*CHno-2,sizeof(Enode));
  CHSplaytree=CHinit();

  temp = S->next;
  
  /* Add v(S) to K. (1) */
  add_point(compute_v(S));                         /* Add the point to K */
  S->v = Kcount-1;                          /* Update the "pointer" in S 
					       to point to v(S) in K     */
  CHinsert(&CHSplaytree,S);
  
  while (temp != S) {
    /* Add v(S) to K. (1) */
    add_point(compute_v(temp));                    /* Add the point to K */
    temp->v=Kcount-1;                    /* Update the "pointer" in temp 
					    to point to v(S) in K        */
    CHinsert(&CHSplaytree,temp);
    temp=temp->next; }
}

/* ********************************************************************* */
/* PROCEDURE: add_edge - adds a new edge to the Voronoi diagram          */

void add_edge(int v1,int v2) {
  int x,y;

  E[Ecount].v1 = v1;                   /* Setup the new edge's endpoints */
  E[Ecount].v2 = v2;

  /*   line(K[v1].p.x,K[v1].p.y,K[v2].p.x,K[v2].p.y); */

 /* getMouse(&x,&y);*/
  
  if (K[v1].e1 == 0) {                         /* Point not in any edges */
    K[v1].e1 = Ecount;
    E[Ecount].p1 = Ecount;                         /* Set up p1 and q2 */
    E[Ecount].q1 = Ecount; } 
  else if (K[v1].e2 == 0) {               /* Point in _one_ edge already */
      K[v1].e2 = Ecount;
      E[Ecount].p1 = K[v1].e1;                       /* Set up p1 and q2 */
      E[Ecount].q1 = K[v1].e1;
      if (v1 == E[K[v1].e1].v1) {           /* Set either p1,q2 or p2,q2 */
	  E[K[v1].e1].p1 = Ecount;           /* in the edge that already */
	  E[K[v1].e1].q1 = Ecount;              /* has v1 as an endpoint */
	}                
      else {                                 
	E[K[v1].e1].p2 = Ecount;
	E[K[v1].e1].q2 = Ecount; } 
    } 
  else {                                     /* Point already in 2 edges */
    K[v1].e3 = Ecount;
    /* more */ 
  }
  
  if (K[v2].e1 == 0) {                         /* Point not in any edges */
    K[v2].e1 = Ecount;
    E[Ecount].p1 = Ecount;                           /* Set up p1 and q2 */
    E[Ecount].q1 = Ecount; } 
  else if (K[v2].e2 == 0) {               /* Point in _one_ edge already */
    K[v2].e2 = Ecount;
    E[Ecount].p1 = K[v2].e1;                         /* Set up p1 and q2 */
    E[Ecount].q1 = K[v2].e1;
    if (v2 == E[K[v2].e1].v1) {             /* Set either p1,q2 or p2,q2 */
      E[K[v2].e1].p1 = Ecount;               /* in the edge that already */
      E[K[v2].e1].q1 = Ecount;                  /* has v2 as an endpoint */
    }                       
    else {                                 
      E[K[v2].e1].p2 = Ecount;
      E[K[v2].e1].q2 = Ecount; }
  } 
  else {                                     /* Point already in 2 edges */
    K[v2].e3 = Ecount;
    /* more */ 
  }
  Ecount++;
}

/* ********************************************************************* */
/* PROCEDURE: maximize_radius_and_angle - ???????                        */

CHpoints *maximize_radius_and_angle(CHpoints *S) {
  CHpoints *p1,*p2,*p3;
  key key1,key2;
  p2=CHdelete_max(&CHSplaytree);
  p1=before(p2);
  p3=next(p2);
  key1.radius=radius2(p1->node,
		      centre(before(p1)->node,p1->node,p2->node));
  key1.angle=angle(before(p1),p1,p2);
  key1.number=p1->number;
  CHdelete(&CHSplaytree,key1); /* delete before(p) */
  key2.radius=radius2(p3->node,
		      centre(p2->node,p3->node,next(p3)->node));
  key2.angle=angle(p2,p3,next(p3));
  key2.number=p3->number;
  CHdelete(&CHSplaytree,key2); /* delete next(p) */
  return p2;
}

/* ********************************************************************* */
/* PROCEDURE: draw_sec - computes the sec and draws it                   */

void draw_sec(CHpoints *p) {
  dpoint c;
  CHpoints *p1,*p2,*p3;
  double radius;
  
  if ((length2(before(p)->node,p->node) > 
       length2(p->node,next(p)->node)) &&
      (length2(before(p)->node,p->node) > 
       length2(before(p)->node,next(p)->node)))
    p2=next(p);  /* the angle at next(p) is the biggest */
  else if ((length2(p->node,next(p)->node) > 
	    length2(before(p)->node,next(p)->node)) &&
	   (length2(p->node,next(p)->node) > 
	    length2(p->node,before(p)->node)))
    p2=before(p); /* the angle at before(p) is the biggest */
  else
    p2=p; /* the angle at p is the biggest */
  p1=before(p2);
  p3=next(p2);

  if (angle(p1,p2,p3)<0) {
    c.x=(midpoint(p1->node,p3->node)).x;      /* center is midpoint of */
    c.y=(midpoint(p1->node,p3->node)).y;      /* p1 and p3             */
    radius=sqrt((double)length2(p1->node,p3->node))/2.00;    
  }
  else {
    c=centre(p1->node,p2->node,p3->node);
    radius=sqrt((double)radius2(p->node,c));
  }
  printf("The center is (%d,%d)\n",(int)c.x,(int)c.y);
  printf("The radius is %9.2f\n",radius);
}	
	   
/* ********************************************************************* */
/* PROCEDURE: alg2 - Main procedure of the algorithm                     */

void alg2() {
  point c;
  dpoint dc;
  CHpoints *p,*q;  

  /* For all p in S add v(p) to K and set up v(p) for each line segment */
  /* p next(p), and count the number of points in S.                    */
  add_infinit_points_to_K(S);
  if (CHno>2) {
    while (CHno>2){
      p=maximize_radius_and_angle(S);
      if (CHno==3)
	draw_sec(p); 
      q=before(p);
      dc=centre(q->node,p->node,next(p)->node);
      c.x= (int)dc.x;
      c.y= (int)dc.y;
      add_point(c);
      add_edge(Kcount-1,p->v);                       /* add_edge(c,v(p)) */
      add_edge(Kcount-1,q->v);                       /* add_edge(c,v(q)) */
      q->v=Kcount-1;                                 /* K[Kcount-1] is c */
      q->next=next(p);      
      next(q)->prev=q;      
      /* Make sure that we don't loose the handle to the circular list S 
	 by making a point is S the new handle                           */
      S=q;
      CHno--;
      /* re-insert before(P) and next(p) with a new radius & angle */
      CHinsert(&CHSplaytree,next(p));
      CHinsert(&CHSplaytree,before(p));
    }
    add_edge(q->v,(next(q))->v);
  }
  else if (CHno=2) 
    add_edge(S->v,(next(S))->v);
  free(S);
  CHfree_tree(CHSplaytree);  
}

/* ********************************************************************* */
/* PROCEDURE: construct_vor - call the procedures to construct the
              voronoi diagram                                            */

void construct_vor() {
  S = construct_ch();
  number_points(S);
  alg2();
  clean_up();
}

/* ********************************************************************* */
/* PROCEDURE: main - starts the program                                  */

int main(int argc, char *argv[])
{
  get_file();
  construct_vor();
  return 0;
}
