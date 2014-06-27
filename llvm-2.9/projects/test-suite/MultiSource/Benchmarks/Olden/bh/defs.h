/* For copyright information, see olden_v1.0/COPYRIGHT */

/*
 * DEFS: include file for tree-code sources.
 * Copyright (c) 1991, Joshua E. Barnes, Honolulu, HI.
 * 	    It's free because it's yours.
 */

#ifdef TORONTO
#include <stdio.h>
#define chatting printf
#define PLAIN
#define LOCAL(xxx) xxx
#define local
#define MLOCAL(xxx);
#define NOTEST();
#define RETEST();
/*#define isnan(xxx) 0*/
#define PID(xxx) 0
int NumNodes;
#endif


#include "stdinc.h"

#define THREEDIM
#include "vectmath.h"

#define MAX_NUM_NODES 64
extern double fabs();


/*
 * BODY and CELL data structures are used to represent the tree:
 *
 *         +-----------------------------------------------+
 * root--> | CELL: mass, pos, quad, /, o, /, /, /, /, o, / |
 *         +---------------------------|--------------|----+
 *                                     |              |
 *    +--------------------------------+              |
 *    |                                               |
 *    |    +--------------------------------+         |
 *    +--> | BODY: mass, pos, vel, acc, phi |         |
 *         +--------------------------------+         |
 *                                                    |
 *    +-----------------------------------------------+
 *    |
 *    |    +-----------------------------------------------+
 *    +--> | CELL: mass, pos, quad, o, /, /, o, /, /, o, / |
 *         +------------------------|--------|--------|----+
 *                                 etc      etc      etc
 */

/*
 * NODE: data common to BODY and CELL structures.
 */

typedef struct node{
    short type;                 /* code for node type */
    real mass;                  /* total mass of node */
    vector pos;			/* position of node */
    int proc;                   /* parent's processor number */
    int new_proc;               /* processor where this node will reside */
#ifdef JUMP
    struct node * next_few_node;
#endif
} node, *nodeptr;

/***
#define Type(x) (((nodeptr) (x))->type)
#define Mass(x) (((nodeptr) (x))->mass)
#define Pos(x)  (((nodeptr) (x))->pos)
#define Proc(x) (((nodeptr) (x))->proc)
#define New_Proc(x) (((nodeptr) (x))->new_proc)
***/
#define Type(x) ((x)->type)
#define Mass(x) ((x)->mass)
#define Pos(x)  ((x)->pos)
#define Proc(x) ((x)->proc)
#define New_Proc(x) ((x)->new_proc)

/*
 * BODY: data structure used to represent particles.
 */

#define BODY 01                 /* type code for bodies */

typedef struct bnode *bodyptr;

typedef struct bnode {
    short type;
    real mass;                  /* mass of body */
    vector pos;                 /* position of body */
    int proc;            /* parent's processor number */
    int new_proc;
#ifdef JUMP
    struct node * next_few_node;
#endif
    vector vel;                 /* velocity of body */
    vector acc;			/* acceleration of body */
    vector new_acc;
    real phi;			/* potential at body */
    bodyptr next;              /* pointer to next point */
    bodyptr proc_next;              /* pointer to next point */
} body;

/***
#define Vel(x)  (((bodyptr) (x))->vel)
#define Acc(x)  (((bodyptr) (x))->acc)
#define New_Acc(x)  (((bodyptr) (x))->new_acc)
#define Phi(x)  (((bodyptr) (x))->phi)
#define Next(x) (((bodyptr) (x))->next)
#define Proc_Next(x) (((bodyptr) (x))->proc_next)
***/
#define Vel(x)  ((x)->vel)
#define Acc(x)  ((x)->acc)
#define New_Acc(x)  ((x)->new_acc)
#define Phi(x)  ((x)->phi)
#define Next(x) ((x)->next)
#define Proc_Next(x) ((x)->proc_next)

/*
 * CELL: structure used to represent internal nodes of tree.
 */

#define CELL 02                 /* type code for cells */

#define NSUB (1 << NDIM)        /* subcells per cell */

typedef struct cnode *cellptr; 

typedef struct cnode {
    short type;
    real mass;                  /* total mass of cell */
    vector pos;                 /* cm. position of cell */
    int proc;
    int new_proc;
#ifdef JUMP
    struct node * next_few_node;
#endif
    nodeptr subp[NSUB];         /* descendents of cell */
    cellptr next;               /* for free list */

} cell;


/***
#define Subp(x) (((cellptr) (x))->subp)
#define FL_Next(x) (((cellptr) (x))->next)
***/
#define Subp(x) ((x)->subp)
#define FL_Next(x) ((x)->next)

typedef struct {
  real rmin[3];               
  real rsize;
  nodeptr root;
  bodyptr bodytab[MAX_NUM_NODES];
  bodyptr bodiesperproc[MAX_NUM_NODES];
} tree, *treeptr;

#define Root(t) ((t)->root)
#define Rmin(t) ((t)->rmin)
#define Rsize(t) ((t)->rsize)
#define Bodytab(t) ((t)->bodytab[0])





typedef struct {
  real tnow;
  real tout;
  int nsteps;
} timerecord, *timeptr;

#define Tnow(t) ((t)->tnow)
#define Tout(t) ((t)->tout)
#define Nsteps(t) ((t)->nsteps)

typedef struct {
  int xp[NDIM];
  bool inb;
} icstruct;

typedef struct {
  double v[NDIM];
} vecstruct;
    

/*
 * ROOT: origin of tree; declared as nodeptr for tree with only 1 body.
 */

global nodeptr root;

/*
 * Integerized coordinates: used to mantain body-tree.
 */

global vector rmin;		/* lower-left corner of coordinate box      */

global real xxxrsize;		/* side-length of integer coordinate box    */

#define IMAX_SHIFT (8 * sizeof(int) - 2)
#define IMAX  (1 << (8 * sizeof(int) - 2))    /* highest bit of int coord */

/*
 * Parameters and results for gravitational calculation.
 */


/* Former global variables.  convert to #defines   */

#define eps 0.05 		/* potential softening parameter            */
#define tol 1.00 		/* accuracy parameter: 0.0 => exact         */
#define NSTEPS  10
#define fcells 0.75		/* ratio of cells/bodies allocated          */




typedef struct {
  real mtot;                /* total mass of N-body system */
  real etot[3];             /* binding, kinetic, potential energy */
  matrix keten;		/* kinetic energy tensor */
  matrix peten;		/* potential energy tensor */
  vector cmphase[2];	/* center of mass coordinates */
  vector amvec;		/* angular momentum vector */
} ostruct;



#define assert(b,n) if (!(b)) {chatting("Assertion Failure #%d", (n)); abort();}


#define TASKLIMIT 256
extern int nbody;

#ifndef TORONTO
#include "future-cell.h"
#define exit __ShutDown
#endif





