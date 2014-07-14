/* For copyright information, see olden_v1.0/COPYRIGHT */


/*
 * CODE.C: export version of hierarchical N-body code.
 * Copyright (c) 1991, Joshua E. Barnes, Honolulu, HI.
 * 	    It's free because it's yours.
 */

#define global  /* nada */

#include "defs.h"
#include "code.h"


int nbody;

double sqrt(), xrand(), my_rand();
real pow();
extern icstruct intcoord(bodyptr p, treeptr t);
extern int BhDebug;

void computegrav(treeptr t, int nstep);
nodeptr maketree(bodyptr btab, int nbody, treeptr t, int nsteps, int proc);
void vp(bodyptr q, int nstep);

void gravstep(real rsize, nodeptr rt, bodyptr p, int nstep, real dthf);
void ptree(nodeptr n, int level);

typedef struct {
	vector cmr;
	vector cmv;
	bodyptr list;
   bodyptr tail;
} datapoints;


bodyptr testdata();
datapoints uniform_testdata(int proc, int nbody, int seedfactor);
void stepsystem(treeptr t, int nstep);
treeptr old_main();
void my_free(nodeptr n);
bodyptr ubody_alloc(int p);
bodyptr movebodies(bodyptr list, int proc);
void freetree(nodeptr n);
void freetree1(nodeptr n);

int arg1;

/* Used to setup runtime system, get arguments-- see old_main */
int main(int argc, char **argv) {
  treeptr t;

  /* Initialize the runtime system */
  dealwithargs(argc, argv);
  chatting("nbody = %d, numnodes = %d\n", nbody, NumNodes);

  t = old_main();
  return 0;
}

/* global! */

/* Main routine from original program */
treeptr old_main() {
  real tnow;
  real tout;
  int i, nsteps, nprocs;
  treeptr t;
  bodyptr bt0,p;
  long t1, t2;
  vector cmr, cmv;
  bodyptr prev=NULL;
  int tmp=0, range=((1<<NDIM) << NDIM) / NumNodes;
  int bodiesper[MAX_NUM_NODES];
  bodyptr ptrper[MAX_NUM_NODES];

  srand(123);					/*   set random generator   */

/* Tree data structure is global, points to root, and bodylist, has size info */
  t = (treeptr)malloc(sizeof(tree));
  Root(t) = NULL;
  t->rmin[0] = -2.0;
  t->rmin[1] = -2.0;
  t->rmin[2] = -2.0;
  t->rsize = -2.0 * -2.0;  /*t->rmin[0];*/

  CLRV(cmr);
  CLRV(cmv);

/* Creates a list of bodies */
  for (i=0; i < 32; i++)
    {
    datapoints points;
    int processor= i/(32/NumNodes);

    points=uniform_testdata(processor, nbody/32, i+1);
    
    t->bodytab[i]=points.list;
    if (prev) 
       Next(prev)=points.list;
    prev = points.tail;
    ADDV(cmr,cmr,points.cmr);
    ADDV(cmv,cmv,points.cmv);
    }

  chatting("bodies created \n");
  DIVVS(cmr, cmr, (real) nbody);		/* normalize cm coords */
  DIVVS(cmv, cmv, (real) nbody);

  for (tmp=0; tmp<MAX_NUM_NODES; tmp++) {
    bodiesper[tmp]=0;
    ptrper[tmp]=NULL;
    }

  /* Recall particles are in lists by processor */
  for (p = t->bodytab[0]; p != NULL; p = Next(p)) {	/* loop over particles */
    icstruct xqic;

    SUBV(Pos(p), Pos(p), cmr);		/*   offset by cm coords    */
    SUBV(Vel(p), Vel(p), cmv);

    xqic = intcoord(p,t);
    tmp = ((old_subindex(xqic, IMAX >> 1) << NDIM) +
               old_subindex(xqic, IMAX >> 2));
    tmp = tmp / range;
    bodiesper[tmp]++;
    Proc_Next(p) = ptrper[tmp];
    ptrper[tmp] = p;
    Proc(p) = tmp;
  }
  for (tmp=0; tmp<NumNodes; tmp++) 
    {
     chatting("Bodies per %d = %d\n",tmp ,bodiesper[tmp]);
     t->bodiesperproc[tmp]=ptrper[tmp];
    }

#ifdef DEBUG
  { int i=0;
    bodyptr p = t->bodytab[0];
  
    for (; i < nbody; i++, p=Next(p))
      printf("%d -- %f %f %f\n", i, Pos(p)[0], Pos(p)[1],
	     Pos(p)[2]);
  }
#endif
  tmp = 0;
  /* t1 = mclock(); */
  tnow = 0.0;
  i = 0;
/* Go through sequence of iterations */
  nsteps = NSTEPS;
  /*chatting("About to perform %d iters from %f to %f by %f\n",*/
            /*nsteps,tnow,tstop,dtime);*/

  while ((tnow < tstop + 0.1*dtime) && (i < nsteps)) {
    stepsystem(t, i);     /*   advance N-body system  */
    tnow = tnow + dtime;
    /*chatting("tnow = %f sp = 0x%x\n", tnow, __getsp());*/
    i++;
  }

  /* t2 = mclock(); */
#ifdef DEBUG
  { int i=0;
    bodyptr p = t->bodytab[0];
  
    for (; i < nbody; i++, p=Next(p))
      printf("%d -- %f %f %f\n", i, Pos(p)[0], Pos(p)[1],
	     Pos(p)[2]);
  }
#endif


/* Return the tree to calling routine */
  return t;
}

/* Use 1 of testdata, uniform_testdata */

/*
 * TESTDATA: generate Plummer model initial conditions for test runs,
 * scaled to units such that M = -4E = G = 1 (Henon, Hegge, etc).
 * See Aarseth, SJ, Henon, M, & Wielen, R (1974) Astr & Ap, 37, 183.
 */

#define MFRAC  0.999		/* mass cut off at MFRAC of total */

/* don't use this unless it is fixed on random numbers, &c */
bodyptr testdata()
{
    real rsc, vsc, r, v, x, y;
    vector cmr, cmv;
    bodyptr head, p, prev;
    register int i;
    double temp, t1;
    double seed = 123.0;
    register int k;
    double rsq, rsc1;
    real rad;

    assert(0,99);
    rsc = 3 * PI / 16;				/* set length scale factor  */
    vsc = sqrt(1.0 / rsc);			/* and recip. speed scale   */
    CLRV(cmr);					/* init cm pos, vel         */
    CLRV(cmv);
    head = (bodyptr) ubody_alloc(0);
    prev = head;

    for (i = 0; i < nbody; i++) {	        /* loop over particles      */
        p = ubody_alloc(0);
						/* alloc space for bodies   */
        if (p == NULL)  			/* check space is available */
	  error("testdata: not enough memory\n");	/*   if not, cry            */
        Next(prev) = p;
        prev = p;
	Type(p) = BODY;				/*   tag as a body          */
	Mass(p) = 1.0 / nbody;			/*   set masses equal       */
	seed = my_rand(seed);
	t1 = xrand(0.0, MFRAC, seed);
	temp = pow(t1,	                        /*   pick r in struct units */
			 -2.0/3.0) - 1;
	r = 1 / sqrt(temp);

	/*   pick scaled position   */
	rad = rsc * r;
	do {					/* pick point in NDIM-space */
	  for (k = 0; k < NDIM; k++) {		/*   loop over dimensions   */
	    seed = my_rand(seed);
	    Pos(p)[k] = xrand(-1.0, 1.0, seed); /* pick from unit cube  */
	  }
	  DOTVP(rsq, Pos(p), Pos(p));		/*   compute radius squared */
	} while (rsq > 1.0);                	/* reject if outside sphere */
	rsc1 = rad / sqrt(rsq);		/* compute scaling factor   */
	MULVS(Pos(p), Pos(p), rsc1);		/* rescale to radius given  */

	ADDV(cmr, cmr, Pos(p));			/*   add to running sum     */
	do {					/*   select from fn g(x)    */
	  seed = my_rand(seed);
	  x = xrand(0.0, 1.0, seed);   		/*     for x in range 0:1   */
	  seed = my_rand(seed);
	  y = xrand(0.0, 0.1, seed);  		/*     max of g(x) is 0.092 */
	} while (y > x*x * pow(1 - x*x, 3.5));	/*   using von Neumann tech */
	v = sqrt(2.0) * x / pow(1 + r*r, 0.25);	/*   find v in struct units */

	/*   pick scaled velocity   */
	rad = vsc*v;

	do {					/* pick point in NDIM-space */
	  for (k = 0; k < NDIM; k++)	{	/* loop over dimensions   */
	    seed = my_rand(seed);
	    Vel(p)[k] = xrand(-1.0, 1.0, seed);	/* pick from unit cube  */
	  }
	  DOTVP(rsq, Vel(p), Vel(p));		/*   compute radius squared */
	} while (rsq > 1.0);                	/* reject if outside sphere */
	rsc1 = rad / sqrt(rsq);		/* compute scaling factor   */
	MULVS(Vel(p), Vel(p), rsc1);		/* rescale to radius given  */
	ADDV(cmv, cmv, Vel(p));			/*   add to running sum     */
    }



    Next(prev) = NULL;                          /*   mark end of the list   */
    head = Next(head);                          /*   toss the dummy node    */

    DIVVS(cmr, cmr, (real) nbody);		/* normalize cm coords      */
    DIVVS(cmv, cmv, (real) nbody);
    for (p = head; p != NULL; p = Next(p)) {	/* loop over particles      */
	SUBV(Pos(p), Pos(p), cmr);		/*   offset by cm coords    */
	SUBV(Vel(p), Vel(p), cmv);
    }
   return head;
}

/*
 * STEPSYSTEM: advance N-body system one time-step.
 */
extern int EventCount;

void stepsystem(treeptr t, int nstep) {
  bodyptr bt, bt0, q;
  int i;
  nodeptr root;

  int barge,cflctdiff,misstemp,diff;
  /*unsigned long t5, t1, t2, t3, t4; */

  /*chatting("Entered stepsystem with t = 0x%x\n",t);*/
  if (Root(t) != NULL) {
    freetree1(Root(t));
    Root(t) = NULL;
  }

  /*chatting("Tree freed\n");*/

  root = maketree(bt, nbody, t, nstep, 0);
  /*chatting("Done maketree\n");*/
  /*BhDebug = 0;*/
  Root(t)=root;

  computegrav(t, nstep);
  /*chatting("Done cg\n");*/

  vp(t->bodiesperproc[0],nstep);
}


void freetree1(nodeptr n)
{
  unsigned long t1, t2;
  int foo;

  freetree(n);
}

  
void freetree(nodeptr n)
{
  register nodeptr r;
  register int i;
  
  /*NOTEST();*/
  if ((n == NULL) || (Type(n) == BODY))
    return;

  /* Type(n) == CELL */
  for (i=NSUB-1; i >= 0; i--) {
    r = Subp((cellptr) n)[i];
    if (r != NULL) {
      freetree(r);
    }
  }

  my_free(n);
  RETEST();
}


nodeptr cp_free_list = NULL;
bodyptr bp_free_list = NULL;


/* should never be called with NULL */
void my_free(nodeptr n)
{
  if (Type(n) == BODY) {
    Next((bodyptr) n) = bp_free_list;
    bp_free_list = (bodyptr) n;
  }
  else /* CELL */ {
    FL_Next((cellptr) n) = (cellptr) cp_free_list;
    cp_free_list = n;
  }
}
    

bodyptr ubody_alloc(int p)
{ register bodyptr tmp;

  tmp = (bodyptr)malloc(sizeof(body));

  Type(tmp) = BODY;
  Proc(tmp) = p;
  Proc_Next(tmp) = NULL;
  New_Proc(tmp) = p;
  return tmp;
  
}


cellptr cell_alloc(int p)
{ register cellptr tmp;
  register int i;

  if (cp_free_list != NULL) {
    tmp = (cellptr) cp_free_list;
    cp_free_list = (nodeptr) FL_Next((cellptr) cp_free_list);
  }
  else 
    {
      tmp = (cellptr)malloc(sizeof(cell));
    }
  Type(tmp) = CELL;
  Proc(tmp) = p;
  for (i=0; i < NSUB; i++)
    Subp(tmp)[i] = NULL;

  return tmp;
}


#define MFRAC  0.999		/* mass cut off at MFRAC of total */
#define DENSITY 0.5

datapoints uniform_testdata(int proc, int nbodyx, int seedfactor)
{
  datapoints retval;
  real rsc, vsc, r, v, x, y;
  bodyptr head, p, prev;
  register int i;
  double temp, t1;
  double seed = 123.0 * (double) seedfactor;
  register int k;
  double rsq, rsc1;
  real rad;
  real coeff;

  /*NOTEST();*/
  rsc = 3 * PI / 16;				/* set length scale factor  */
  vsc = sqrt(1.0 / rsc);			/* and recip. speed scale   */
  CLRV(retval.cmr);					/* init cm pos, vel         */
  CLRV(retval.cmv);
  head = (bodyptr) ubody_alloc(proc);
  prev = head;

  for (i = 0; i < nbodyx; i++) {	        /* loop over particles      */
    p = ubody_alloc(proc);
						/* alloc space for bodies   */
    if (p == NULL)  			/* check space is available */
      error("testdata: not enough memory\n");	/*   if not, cry            */
    Next(prev) = p;
    prev = p;
    Type(p) = BODY;				/*   tag as a body          */
    Mass(p) = 1.0 / nbodyx;			/*   set masses equal       */
    seed = my_rand(seed);
    t1 = xrand(0.0, MFRAC, seed);
    temp = pow(t1,	                        /*   pick r in struct units */
			 -2.0/3.0) - 1;
    r = 1 / sqrt(temp);
    
    coeff = 4.0; /* exp(log(nbodyx/DENSITY)/3.0); */
    for (k=0; k < NDIM; k++) {
      seed = my_rand(seed);
      r = xrand(0.0, MFRAC, seed);
      Pos(p)[k] = coeff*r;
    }

    ADDV(retval.cmr, retval.cmr, Pos(p));			/*   add to running sum     */
    do {					/*   select from fn g(x)    */
      seed = my_rand(seed);
      x = xrand(0.0, 1.0, seed);   		/*     for x in range 0:1   */
      seed = my_rand(seed);
      y = xrand(0.0, 0.1, seed);  		/*     max of g(x) is 0.092 */
    } while (y > x*x * pow(1 - x*x, 3.5));	/*   using von Neumann tech */
    v = sqrt(2.0) * x / pow(1 + r*r, 0.25);	/*   find v in struct units */

    /*   pick scaled velocity   */
    rad = vsc*v;
    
    do {					/* pick point in NDIM-space */
      for (k = 0; k < NDIM; k++)	{	/* loop over dimensions   */
	seed = my_rand(seed);
	Vel(p)[k] = xrand(-1.0, 1.0, seed);	/* pick from unit cube  */
      }
      DOTVP(rsq, Vel(p), Vel(p));		/*   compute radius squared */
    } while (rsq > 1.0);                	/* reject if outside sphere */
    rsc1 = rad / sqrt(rsq);		/* compute scaling factor   */
    MULVS(Vel(p), Vel(p), rsc1);		/* rescale to radius given  */
    ADDV(retval.cmv, retval.cmv, Vel(p));			/*   add to running sum     */
  }


  Next(prev) = NULL;                          /*   mark end of the list   */
  retval.list = Next(head);                   /*   toss the dummy node    */
  retval.tail = prev;

  RETEST();
  
  return retval;
}
/*
 * GRAV.C: routines to compute gravity. Public routines: hackgrav().
 * Copyright (c) 1991, Joshua E. Barnes, Honolulu, HI.
 * 	    It's free because it's yours.
 */

/********
#define global extern

#include "defs.h"
#include "code.h"
#include "mem-ref.h"
*********/

/*extern int NumNodes;*/

typedef struct {
  bodyptr pskip;		/* body to skip in force evaluation */
  vector pos0;			/* point at which to evaluate field */
  real phi0;			/* computed potential at pos0       */
  vector acc0;			/* computed acceleration at pos0    */
} hgstruct, *hgsptr;

hgstruct gravsub(nodeptr p, hgstruct hg);
hgstruct walksub(nodeptr p, real dsq, real tolsq, hgstruct hg, int level);
bool subdivp(nodeptr p, real dsq, real tolsq, hgstruct hg);
double sqrt(double arg);
void grav(real rsize, nodeptr rt, bodyptr q, int nstep, real dthf);
void vp(bodyptr q, int nstep);
void hackgrav(bodyptr p, real rsize, nodeptr rt);

void computegrav(treeptr t, int nstep)
{  register int i;
   real rsize;
   real dthf;
   nodeptr root;
   bodyptr blist;

   /* loop over particles   */
   rsize = Rsize(t);

   dthf = 0.5 * dtime;

   for (i=NumNodes-1; i>=0; i--) {
     root = Root(t);
     blist = t->bodiesperproc[i];
     grav(rsize,root,blist,nstep,dthf);
   }
}


void grav(real rsize, nodeptr rt, bodyptr bodies, int nstep, real dthf)
{
  register bodyptr p, q;
  int i=0;


  /* get it to move to the right processor! */
  if (bodies!= NULL) {
    bodyptr foo = bodies;
  }
  q = bodies;

  NOTEST();

  while (q!=NULL) {
    gravstep(rsize, rt, q, nstep, dthf);
    q=Proc_Next(q);
    i++;
  }
  /*chatting("computed gravity on %d particles\n",i);*/
  RETEST();

}

void vp(bodyptr q, int nstep)
{
  real dthf;
  vector acc1, dacc, dvel, vel1, dpos;

  dthf = 0.5 * dtime;				/* set basic half-step      */

  /* move to the correct processor */
  if (q!= NULL) {
    bodyptr foo = q;
  }

  NOTEST();

  for (; q != NULL; q = Proc_Next(q))  {
    SETV(acc1, New_Acc(q));
    if (nstep > 0) {			/*   past the first step?   */
      SUBV(dacc, acc1, Acc(q));   /*     use change in accel  */
      MULVS(dvel, dacc, dthf);		/*     to make 2nd order    */
      /*ADDV(Vel(q), Vel(q), dvel);*/	/*     correction to vel    */
      ADDV(dvel, Vel(q), dvel);
      SETV(Vel(q), dvel);
    }
    { real p0,p1,p2;
    p0=Pos(q)[0];
    p1=Pos(q)[1];
    p2=Pos(q)[2];
    assert (!isnan(p0),99);
    assert (!isnan(p1),98);
    assert (!isnan(p2),97);
    assert (fabs(p0)<10.0,96);
    assert (fabs(p1)<10.0,95);
    assert (fabs(p2)<10.0,94);
#ifdef DEBUG
    chatting("POSITION:vp--%f,%f,%f\n",p0,p1,p2);
#endif
    }
    SETV(Acc(q), acc1);
    { real p0,p1,p2;
    p0=Acc(q)[0];
    p1=Acc(q)[1];
    p2=Acc(q)[2];
    assert (!isnan(p0),89);
    assert (!isnan(p1),88);
    assert (!isnan(p2),87);
    assert (fabs(p0)<10000.0,86);
    assert (fabs(p1)<10000.0,85);
    assert (fabs(p2)<10000.0,84);
    /*chatting("ACCEL:vp--%f,%f,%f\n",p0,p1,p2);*/
    }

    MULVS(dvel, Acc(q), dthf);	        /*   use current accel'n    */
    { real p0,p1,p2;
    p0=Vel(q)[0];
    p1=Vel(q)[1];
    p2=Vel(q)[2];
    assert (!isnan(p0),79);
    assert (!isnan(p1),78);
    assert (!isnan(p2),77);
    assert (fabs(p0)<10000.0,76);
    assert (fabs(p1)<10000.0,75);
    assert (fabs(p2)<10000.0,74);
    /*chatting("VELOCITY:vp--%f,%f,%f\n",p0,p1,p2);*/
    }
    ADDV(vel1, Vel(q), dvel);		/*   find vel at midpoint   */
    MULVS(dpos, vel1, dtime);		/*   find pos at endpoint   */
    ADDV(dpos, Pos(q), dpos);   	/*   advance position       */
    SETV(Pos(q),dpos);
    ADDV(Vel(q), vel1, dvel);		/*   advance velocity       */
    { real p0,p1,p2;
    p0=Pos(q)[0];
    p1=Pos(q)[1];
    p2=Pos(q)[2];
    assert (!isnan(p0),69);
    assert (!isnan(p1),68);
    assert (!isnan(p2),67);
    assert (fabs(p0)<10000.0,66);
    assert (fabs(p1)<10000.0,65);
    assert (fabs(p2)<10000.0,64);
    /*chatting("vp--%f,%f,%f\n",p0,p1,p2);*/
    }
  }

  RETEST();
}

  
/*
 */
void gravstep(real rsize, nodeptr rt, bodyptr p, int nstep, real dthf)
{

  hackgrav(p, rsize, rt);			/*   compute new acc for p  */
}

/*
 * HACKGRAV: evaluate grav field at a given particle.
 */


void hackgrav(bodyptr p, real rsize, nodeptr rt)
{
  hgstruct hg;
  real szsq;

  NOTEST();
  hg.pskip = p;					/* exclude from force calc. */
  SETV(hg.pos0, Pos(p));			/* eval force on this point */
  hg.phi0 = 0.0;				/* init grav. potential and */
  CLRV(hg.acc0);
  szsq = rsize * rsize;
  hg = walksub(rt, szsq, tol*tol, hg, 0);    /* recursively scan tree    */
  Phi(p) = hg.phi0;				/* stash resulting pot. and */
  /*chatting("hg.acc : %f,%f,%f\n",hg.acc0[0],hg.acc0[1],hg.acc0[2]);*/
  SETV(New_Acc(p), hg.acc0);			/* acceleration in body p   */
  RETEST();
}

/*
 * GRAVSUB: compute a single body-body or body-cell interaction.
 */

hgstruct gravsub(nodeptr p, hgstruct hg)
{
  real drabs, phii, mor3;
  vector ai, quaddr;
  real dr5inv, phiquad, drquaddr;
  vector dr;
  real drsq;


  SUBV(dr, Pos(p), hg.pos0);/*<-- 14.6% load penalty */       /*  find seperation   */
  DOTVP(drsq, dr, dr);			/*   and square of distance */

  drsq += eps*eps;                      /* use standard softening   */
  drabs = sqrt((double) drsq);		/* find norm of distance    */
  phii = Mass(p) / drabs;		/* and contribution to phi  */
  hg.phi0 -= phii;                      /* add to total potential   */
  mor3 = phii / drsq;  			/* form mass / radius qubed */
  MULVS(ai, dr, mor3);			/* and contribution to acc. */
  ADDV(hg.acc0, hg.acc0, ai);           /* add to net acceleration  */

  if(Type(p) == CELL) {                 /* a body-cell interaction? */
#ifdef QUADPOLE				/*   add qpole correction   */
    dr5inv = 1.0/(drsq * drsq * drabs);     /*   form dr ** (-5)        */
    MULMV(quaddr, Quad(p), dr);       /*   form Q * dr            */
    DOTVP(drquaddr, dr, quaddr);            /*   form dr * Q * dr       */
    phiquad = -0.5 * dr5inv * drquaddr;     /*   quad. part of poten.   */
    hg.phi0 = hg.phi0 + phiquad;            /*   increment potential    */
    phiquad = 5.0 * phiquad / drsq;         /*   save for acceleration  */
    MULVS(ai, dr, phiquad);                 /*   components of acc.     */
    SUBV(hg.acc0, hg.acc0, ai);             /*   increment              */
    MULVS(quaddr, quaddr, dr5inv);   
    SUBV(hg.acc0, hg.acc0, quaddr);         /*   acceleration           */
#endif
  }

  return hg;
}

/*
 * HACKWALK: walk the tree opening cells too close to a given point.
 */

/*
 * WALKSUB: recursive routine to do hackwalk operation.
 * p: pointer into body-tree 
 * dsq: size of box squared 
 */



/*
 * SUBDIVP: decide if a node should be opened.
 * Side effects: sets pmem, dr, and drsq.
 * p: body/cell to be tested 
 * dsq: size of cell squared 
 */

 bool subdivp(nodeptr p, real dsq, real tolsq, hgstruct hg)
{
  register nodeptr local_p;
  vector dr;
  vector pos;
  real drsq;

  local_p = (nodeptr)p;
  if (Type(local_p) == BODY) /*<-- 27% load penalty */                  /* at tip of tree? */
    return (FALSE);                           /*   then cant subdivide */

  SUBV(dr, Pos(local_p), hg.pos0); /*<-- 13% load penalty */              /* compute displacement  */   
  DOTVP(drsq, dr, dr); /* <-- 8.6% load penalty */                       /* and find dist squared    */


  return (tolsq * drsq < dsq);                /* use geometrical rule     */
}

/*
 * LOAD.C: routines to create body-tree.  Public routines: maketree().
 * Copyright (c) 1991, Joshua E. Barnes, Honolulu, HI.
 * 	    It's free because it's yours.
 */

double ceil();

bodyptr body_alloc(int p, real p0, real p1, real p2, real v0, real v1, real v2, real a0, real a1, real a2, real mass, bodyptr ob);
bodyptr ubody_alloc(int p);
cellptr cell_alloc(int p);

typedef struct {
  nodeptr tp;
  int num;
} dtstruct;

typedef struct {
  nodeptr tp;
  int num;
  int proc;
} dt1struct;


nodeptr loadtree(bodyptr p, icstruct xpic, nodeptr t, int l, treeptr tr);
void expandbox(bodyptr p, treeptr t, int nsteps, int proc);
icstruct intcoord1(double rp0, double rp1, double rp2,  treeptr t);
icstruct intcoord(bodyptr p,  treeptr t);
int ic_test(bodyptr p, treeptr t);
int subindex(bodyptr p, treeptr t, int l);
int old_subindex(icstruct ic, int l);
real hackcofm(nodeptr q);


int dis2_number(nodeptr n, int prev_bodies, int tnperproc);
void dis_number (nodeptr n);

/*
 *  MAKETREE: initialize tree structure for hack force calculation.
 */

nodeptr maketree(bodyptr btab, int nb, treeptr t, int nsteps, int proc)
{  
  register bodyptr q;
  int tmp;
  nodeptr node1;
  icstruct xqic;
  int holder;

  Root(t) = NULL;
  nbody = nb;


 for (tmp=NumNodes-1; tmp>=0; tmp--) {
  btab = t->bodiesperproc[tmp];

  /*chatting("Entering inner loop \n");*/
  for (q = btab; q != NULL; q=Proc_Next(q)) { 	/* loop over all bodies  */
    if (Mass(q) != 0.0) {			/*   only load massive ones */
      expandbox(q, t, nsteps, proc);   /*     expand root to fit   */
      xqic = intcoord(q,t);
      node1=Root(t);
      node1 = loadtree(q, xqic, node1, IMAX >> 1, t);
      Root(t) = node1;
      /*     insert into tree     */
    } /* if Mass... */
  } /* for q = btab... */
 } /* for NumNodes... */

  /*CMMD_node_timer_clear(2);*/
  /*CMMD_node_timer_start(2);*/
  /*chatting("About to hackcofm\n");*/
  hackcofm(Root(t));				/* find c-of-m coordinates  */
  /*CMMD_node_timer_stop(2);*/
  /*chatting("hackcofm %f\n",CMMD_node_timer_elapsed(2));*/

  return Root(t);
}
  

 

/*
 * New EXPANDBOX: enlarge cubical "box", salvaging existing tree structure.
 * p - body to be loaded 
 * t - tree 
 */

void expandbox(bodyptr p, treeptr t, int nsteps, int proc)       
{
    icstruct ic;
    int k;
    vector rmid;
    cellptr  newt;
    tree tmp;
    real rsize;
    int inbox;

    inbox = ic_test(p, t);
    while (!inbox) {            		/* expand box (rarely)      */
      rsize = Rsize(t);
      /*chatting("expanding box 0x%x, size=%f\n",p,rsize);*/
      assert(rsize<1000.0,999);
      ADDVS(rmid, Rmin(t), 0.5 * rsize);    /*   find box midpoint      */
                                            /*   loop over dimensions   */
/***
      chatting("midpoint:%f,%f,%f\n",rmid[0],rmid[1],rmid[2]);
{
      vector pos;
      pos[0]=Pos(p)[0];
      pos[1]=Pos(p)[1];
      pos[2]=Pos(p)[2];
      chatting("position:%f,%f,%f\n",pos[0],pos[1],pos[2]);
}
      chatting("rsize now = %f\n",rsize);
***/
      for (k=0; k < NDIM; k++)
        if (Pos(p)[k] < rmid[k])            /*     is p left of mid?    */
           {
             real rmin;
             rmin = Rmin(t)[k];
	          Rmin(t)[k] = rmin - rsize;      /*       extend to left     */
           }
      /*chatting("rsize now = %f\n",rsize);*/
      Rsize(t) = 2.0 * rsize;               /*   double length of box   */


      rsize = Rsize(t);
      /*chatting("rsize now = %f\n",rsize);*/
      if (Root(t) != NULL) {                  /*   repot existing tree?   */
	   register int i;
	   newt = cell_alloc(0);

	   ic = intcoord1(rmid[0], rmid[1], rmid[2], t);
	   /*   locate old root cell   */
	   assert(ic.inb, 1);	                /* xmid                     */
	   k = old_subindex(ic, IMAX >> 1);        /*     find old tree index  */
	   Subp(newt)[k] = Root(t);            /*     graft old on new     */
	   Root(t) = (nodeptr) newt;           /*     plant new tree       */
	   inbox = ic_test(p, t);
      } /* if Root... */
    } /* while !inbox */
}

/*
 * New LOADTREE: descend tree and insert particle.
 * p - body to be loaded 
 * xp - integer coordinates of p
 * t - tree
 * l - current level in tree 
 */

nodeptr loadtree(bodyptr p, icstruct xpic, nodeptr t, int l, treeptr tr)
{
  int si;
  cellptr c;
  nodeptr rt;

  if (t == NULL)
     {
      return ((nodeptr) p);
     }
  else {
    assert(l != 0, 2);				/*   dont run out of bits   */
    if (Type(t) == BODY) {        		/*   reached a "leaf"?      */
      int i,j;
      icstruct pic,tic;

      /*chatting("Collision\n");*/
      /*printtree(t); printtree(p);*/
      i = PID(t);
      c = (cellptr) cell_alloc(i);
      si = subindex((bodyptr) t, tr, l); 
     
      Subp(c)[si] = (nodeptr) t;        	/*     put body in cell     */
      t = (nodeptr) c;	        	/*     link cell in tree    */
    }

    si = old_subindex(xpic, l);     /* move down one level      */
    rt = Subp((cellptr) t)[si];
    Subp((cellptr) t)[si] = loadtree(p, xpic, rt, l >> 1, tr);
  }
  return (t);
}


/*
 * INTCOORD: compute integerized coordinates.
 * Returns: TRUE unless rp was out of bounds.
 */
/* called from expandbox */
icstruct intcoord1(double rp0, double rp1, double rp2, treeptr t)
{
    double xsc, floor();
    /*double rmin,rsize;*/
    icstruct ic;

/***
   rmin = t->rmin[0];
   rsize = t->rsize;
    chatting("rp0 %f,rmin %f,rsize %f\n",rp0,rmin,rsize);
***/
    ic.inb = TRUE;					/* use to check bounds      */

    xsc = (rp0 - t->rmin[0]) / t->rsize;    /*   scale to range [0,1)   */
    if (0.0 <= xsc && xsc < 1.0)              /*   within unit interval?  */
      ic.xp[0] = floor(IMAX * xsc);           /*     then integerize      */
    else {                                    /*   out of range           */
      /*chatting("xsc0:%f\n",xsc);*/
      ic.inb = FALSE;                         /*     then remember that   */
      }

    xsc = (rp1 - t->rmin[1]) / t->rsize;    /*   scale to range [0,1)   */
    if (0.0 <= xsc && xsc < 1.0)              /*   within unit interval?  */
      ic.xp[1] = floor(IMAX * xsc);           /*     then integerize      */
    else {                                    /*   out of range           */
      /*chatting("xsc1:%f\n",xsc);*/
      ic.inb = FALSE;                         /*     then remember that   */
      }


    xsc = (rp2 - t->rmin[2]) / t->rsize;    /*   scale to range [0,1)   */
    if (0.0 <= xsc && xsc < 1.0)              /*   within unit interval?  */
      ic.xp[2] = floor(IMAX * xsc);           /*     then integerize      */
    else {                                    /*   out of range           */
      /*chatting("xsc2:%f\n",xsc);*/
      ic.inb = FALSE;                         /*     then remember that   */
      }


    return (ic);
}


/*
 * INTCOORD: compute integerized coordinates.
 * Returns: TRUE unless rp was out of bounds.
 */

icstruct intcoord(bodyptr p, treeptr t)
{
    register double xsc;
    double floor();
    icstruct ic;
    register real rsize;
    vector pos;


    ic.inb = TRUE;					/* use to check bounds      */
    rsize = t->rsize;

    pos[0] = Pos(p)[0];
    pos[1] = Pos(p)[1];
    pos[2] = Pos(p)[2];
 
    /*chatting("Intcoord:%f,%f,%f\n",pos[0],pos[1],pos[2]);*/

    xsc = (pos[0] - t->rmin[0]) / rsize;    /*   scale to range [0,1)   */
    if (0.0 <= xsc && xsc < 1.0)              /*   within unit interval?  */
      ic.xp[0] = floor(IMAX * xsc);           /*     then integerize      */
    else {                                    /*   out of range           */
      ic.inb = FALSE;                         /*     then remember that   */
      ic.xp[0] = 0;
    }


    xsc = (pos[1] - t->rmin[1]) / rsize;    /*   scale to range [0,1)   */
    if (0.0 <= xsc && xsc < 1.0)              /*   within unit interval?  */
      ic.xp[1] = floor(IMAX * xsc);           /*     then integerize      */
    else {                                    /*   out of range           */
      ic.inb = FALSE;                         /*     then remember that   */
      ic.xp[1] = 0;
    }


    xsc = (pos[2] - t->rmin[2]) / rsize;    /*   scale to range [0,1)   */
    if (0.0 <= xsc && xsc < 1.0)              /*   within unit interval?  */
      ic.xp[2] = floor(IMAX * xsc);           /*     then integerize      */
    else {                                     /*   out of range           */
      ic.inb = FALSE;                         /*     then remember that   */
      ic.xp[2] = 0;
    }


    /*chatting("Returning %d:%d:%d, ic.inb=%d\n",ic.xp[0],ic.xp[1],ic.xp[2],ic.inb);*/
    return (ic);
}


int ic_test(bodyptr p, treeptr t)
{
    double xsc, rsize, floor();
    int result;
    vector pos;

    result = TRUE;					/* use to check bounds      */

    pos[0] = Pos(p)[0];
    pos[1] = Pos(p)[1];
    pos[2] = Pos(p)[2];
    rsize = t->rsize;

    xsc = (pos[0] - t->rmin[0]) / rsize;    /*   scale to range [0,1)   */
    if (!(0.0 <= xsc && xsc < 1.0))         /*   within unit interval?  */
      result = FALSE;                       /*     then remember that   */
    /*if (result==FALSE) chatting("rsize=%f,xsc=%f\n",rsize,xsc);*/

    xsc = (pos[1] - t->rmin[1]) / rsize;    /*   scale to range [0,1)   */
    if (!(0.0 <= xsc && xsc < 1.0))         /*   within unit interval?  */
      result = FALSE;                       /*     then remember that   */
    /*if (result==FALSE) chatting("rsize=%f,xsc=%f\n",rsize,xsc);*/

    xsc = (pos[2] - t->rmin[2]) / rsize;    /*   scale to range [0,1)   */
    if (!(0.0 <= xsc && xsc < 1.0))         /*   within unit interval?  */
      result = FALSE;                       /*     then remember that   */
    /*if (result==FALSE) chatting("rsize=%f,xsc=%f\n",rsize,xsc);*/

    /*if (result==FALSE)
     { real size;
       vector mid;
       SETV(mid,t->rmin);
       size = t->rsize;
       chatting("rsize =%f\n",size);
       chatting("minimum:%f,%f,%f\n",mid[0],mid[1],mid[2]);
       chatting("position:%f,%f,%f\n",pos[0],pos[1],pos[2]);
     }*/
    return (result);
}




/*
 * SUBINDEX: determine which subcell to select.  Rolled intcoord & subindex together.
 */

int subindex(bodyptr p, treeptr t , int l)
{
    register int i, k;
    register real rsize;
    double xsc, floor();
    int xp[NDIM];
    vector pos;

    pos[0] = Pos(p)[0];
    pos[1] = Pos(p)[1];
    pos[2] = Pos(p)[2];

    rsize = t->rsize;

    xsc = (pos[0] - t->rmin[0]) / rsize;     /*   scale to range [0,1)   */
    assert((0.0 <= xsc) && (xsc < 1.0), 5);
    xp[0] = floor(IMAX * xsc);                  /*   then integerize      */

    xsc = (pos[1] - t->rmin[1]) / rsize;     /*   scale to range [0,1)   */
    assert((0.0 <= xsc) && (xsc < 1.0), 6);
    xp[1] = floor(IMAX * xsc);                  /*   then integerize      */

    xsc = (pos[2] - t->rmin[2]) / rsize;     /*   scale to range [0,1)   */
    assert((0.0 <= xsc) && (xsc < 1.0), 7);
    xp[2] = floor(IMAX * xsc);                  /*   then integerize      */


    i = 0;                                      /* sum index in i           */
    for (k = 0; k < NDIM; k++)                  /* check each dimension     */
        if (xp[k] & l)                          /*   if beyond midpoint     */
            i += NSUB >> (k + 1);               /*     skip over subcells   */

    return (i);
}



int old_subindex(icstruct ic, int l)
{
    register int i, k;

    i = 0;                                      /* sum index in i           */
    for (k = 0; k < NDIM; k++)                  /* check each dimension     */
        if (ic.xp[k] & l)                       /*   if beyond midpoint     */
            i += NSUB >> (k + 1);               /*     skip over subcells   */
    return (i);
}


/*
 * HACKCOFM: descend tree finding center-of-mass coordinates.
 */

real hackcofm(nodeptr q)
{
    register int i;
    register nodeptr r;
    vector tmpv;
    vector tmp_pos;
    register real mq, mr;

    /*chatting("Entered hackcofm, q=0x%x,fp=0x%x,sp=0x%x\n",q,__getfp(),__getsp());*/
    if (Type(q) == CELL) {                      /* is this a cell?          */
      mq = 0.0;
      CLRV(tmp_pos);				/*   and c. of m.           */
      for (i=0; i < NSUB; i++) {
	     r = Subp((cellptr) q)[i];
	     if (r != NULL) {
	       mr = hackcofm(r);
	       mq = mr + mq;
	       MULVS(tmpv, Pos(r), mr);   /*       find moment        */
	       ADDV(tmp_pos, tmp_pos, tmpv);     /*       sum tot. moment    */
	     }
      }

      Mass(q) = mq;
      NOTEST();
      Pos(q)[0] = tmp_pos[0];
      Pos(q)[1] = tmp_pos[1];
      Pos(q)[2] = tmp_pos[2];
      DIVVS(Pos(q), Pos(q), Mass(q));         /*   rescale cms position   */
      RETEST();
      /*chatting("leaving hackcofm cell,q=0x%x,fp=0x%x,sp=0x%x\n",*/
        /*q,__getfp(),__getsp());*/
      return mq;
    }
    /*chatting("0x%x::Hackcofm mass = %f, type = %d\n",q,Mass(q),Type(q));*/
    /*chatting("0x%x:position:%f::%f::%f\n",q,Pos(q)[0],Pos(q)[1],Pos(q)[2]);*/
    mq = Mass(q);
    /*chatting("leaving hackcofm body\n");*/
    /*chatting("leaving hackcofm body,q=0x%x,fp=0x%x,sp=0x%x\n",*/
        /*q,__getfp(),__getsp());*/
    return mq;
  }


/* preorder traversal of the tree pointed to by n. */

void printtree(nodeptr n)
{ ptree(n, 0);
}

void ptree(nodeptr n, int level)
{ 
  nodeptr r;
  
  
  if (n != NULL) {
    if (Type(n) == BODY) {
      chatting("%2d BODY@%x %f, %f, %f\n", level, n, Pos(n)[0], Pos(n)[1], Pos(n)[2]);
    }
    else /* CELL */ {
      int i;

      chatting("%2d CELL@%x %f, %f, %f\n", level, n,Pos(n)[0], Pos(n)[1], Pos(n)[2]);
      for (i = 0; i < NSUB; i++) {
	r = Subp((cellptr) n)[i];
	ptree(r, level+1);
      }
    }
  }
  else 
    printf("%2d NULL TREE\n", level);
}

      

typedef struct {
  int bits;
  int split;
  cellptr new; 
  nodeptr non_local[NSUB];
} dt3_struct;


void dis_number (nodeptr n)
{
  int tnperproc = (int) ceil ( (double) nbody/NumNodes);

  dis2_number(n, -1, tnperproc);
}

int dis2_number(nodeptr n, int prev_bodies, int tnperproc)
{

  if (n == NULL) 
    return prev_bodies;

  else if (Type(n) == BODY) {
    New_Proc(n) = (prev_bodies+1)/tnperproc;
    return prev_bodies + 1;
  }

  else { /* cell */
    register int i;
    register nodeptr r;

    /*NOTEST();*/
    for (i=0; i < NSUB; i++) {
      r = Subp((cellptr) n)[i];
      prev_bodies = dis2_number(r, prev_bodies, tnperproc);
    }

    RETEST();
    return prev_bodies;
  }
}    

