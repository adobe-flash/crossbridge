/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <stdio.h>
#include <stdlib.h>
#include "defines.h"

#if defined(__linux__)
#include <malloc.h>
#endif


VERTEX_PTR *vp ;
struct VERTEX *va ;
EDGE_PTR *next ;
VERTEX_PTR *org ;
int num_vertices, num_edgeparts, stack_size ;
int to_lincoln, to_off, to_3d_out, to_color, voronoi, delaunay, interactive, ahost;
char *see;
int NumNodes, NDim;

int flag;

QUAD_EDGE connect_left(a, b)
QUAD_EDGE a,b;
{
  VERTEX_PTR t1,t2;
  register QUAD_EDGE ans,lnexta;

/*printf("begin connect_left\n");*/
  t1=dest(a);
  lnexta=lnext(a); 
  t2=orig(b);
  ans = makeedge(t1,t2/*dest(a), orig(b)*/);
  splice(ans, lnexta);
  splice(sym(ans), b);
/*printf("end connect_left\n");*/
  return(ans);
}

QUAD_EDGE connect_right(a, b)
QUAD_EDGE a,b;
{
  VERTEX_PTR t1,t2;
  register QUAD_EDGE ans, oprevb;

/*printf("begin connect_right\n");*/
  t1=dest(a);
  t2=orig(b);
  oprevb=oprev(b); 
  ans = makeedge(t1,t2/*dest(a), orig(b)*/);
  splice(ans, sym(a));
  splice(sym(ans), oprevb);
/*printf("end connect_right\n");*/
  return(ans);
}

void deleteedge(e)
     /*disconnects e from the rest of the structure and destroys it. */
QUAD_EDGE e;
{
  QUAD_EDGE f;
/*printf("begin delete_edge 0x%x\n",e);*/
    f=oprev(e);
    splice(e, f);
    f=oprev(sym(e));
    splice(sym(e),f);
    free_edge(e);
/*printf("end delete_edge\n");*/
}

/****************************************************************/
/*	Top-level Delaunay Triangulation Procedure              */
/****************************************************************/

QUAD_EDGE build_delaunay_triangulation(tree,extra)
    /* builds delaunay triangulation.
       va is an array of vertices, from 0 to size.  Each vertex consists of
       a vector and a data pointer.   edge is a pointer to an
       edge on the convex hull of the constructed delaunay triangulation. */

     VERTEX_PTR tree,extra;
{
    EDGE_PAIR retval;

    retval=build_delaunay(tree,extra);
    return retval.left;
}

VERTEX_PTR get_low(tree)
     register VERTEX_PTR tree;
{
  register VERTEX_PTR temp;
  while((temp=tree->left)) tree=temp;             /* 3% load penalty */
  return tree;
}

/****************************************************************/
/*	Recursive Delaunay Triangulation Procedure              */
/*	Contains modifications for axis-switching division.     */
/****************************************************************/

EDGE_PAIR build_delaunay(VERTEX_PTR tree, VERTEX_PTR extra)
{
    QUAD_EDGE a,b,c,ldo,rdi,ldi,rdo;
    EDGE_PAIR retval;
    register VERTEX_PTR maxx, minx;
    VERTEX_PTR s1, s2, s3;

    EDGE_PAIR delleft, delright;

    if (tree && tree->right)                /* <----------------------- 3% load penalty */
      {
        /* more than three elements; do recursion */
	minx = get_low(tree); maxx = extra;
	delright = build_delaunay(tree->right,extra);
	delleft = build_delaunay(tree->left, tree);
	ldo = delleft.left; ldi=delleft.right;
	rdi=delright.left; rdo=delright.right;
	retval=do_merge(ldo, ldi, rdi, rdo);
	ldo = retval.left;
	rdo = retval.right;
	while (orig(ldo) != minx) {ldo = rprev(ldo); }
	while (orig(rdo) != maxx) {rdo = lprev(rdo); }
	retval.left = ldo;
	retval.right = rdo;
    }
    else if (!tree)
      {
	printf("ERROR: Only 1 point!\n"); 
	exit(-1);
      }
    else if (!tree->left) {	/* two points */
	a = makeedge(tree, extra);
	retval.left =  a;
	retval.right = sym(a);
    }
    else { /*  tree->left, !tree->right  */	/* three points */
	/* 3 cases: triangles of 2 orientations, and 3 points on a line. */
   s1 = tree->left;
   s2 = tree;
   s3 = extra;
	a = makeedge(s1, s2);
	b = makeedge(s2, s3);
	splice(sym(a), b);
	c = connect_left(b, a);
	if (ccw(s1, s3, s2)) {
	    retval.left = sym(c);
	    retval.right = c;
	}
	else {
	    retval.left =  a;
	    retval.right = sym(b);
	    if (!ccw(s1, s2, s3)) deleteedge(c);    /* colinear */
	}
    }
return retval;
}

/****************************************************************/
/*	Quad-edge storage allocation                            */
/****************************************************************/
QUAD_EDGE next_edge, avail_edge;

#define NYL NULL

void delete_all_edges() { next_edge= 0; avail_edge = NYL;}

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__MINGW32__)
#define MEMALIGN_IS_NOT_AVAILABLE
#endif

/* memalign() on my SGI doesn't work. Thus, I have to write my own */
void* myalign(int align_size, int alloc_size)
{   
#ifdef MEMALIGN_IS_NOT_AVAILABLE
    char* base = (char*)malloc(alloc_size + align_size);
#else
    char* base = (char*)memalign(align_size, alloc_size);
#endif
    void *Result;
    if (base == NULL){
        printf("myalign() failed\n");
        exit(-1);
    }
#ifdef MEMALIGN_IS_NOT_AVAILABLE
    return (void*)(base + align_size - ((uptrint)base % align_size));
#else
    return base;
#endif
}



QUAD_EDGE alloc_edge() {
  QUAD_EDGE ans;

  if (avail_edge == NYL) {
    ans = (QUAD_EDGE)myalign(4*(sizeof(struct edge_rec)),
                             4*(sizeof(struct edge_rec)));
    if ((uptrint)ans & ANDF) {
      printf("Aborting in alloc_edge, ans = 0x%p\n", ans);
      exit(-1);
    }
  } else
    ans = (QUAD_EDGE) avail_edge, avail_edge = onext(avail_edge);

  ans[0].wasseen = 0;
  ans[1].wasseen = 0;
  ans[2].wasseen = 0;
  ans[3].wasseen = 0;
  return ans;
}

void free_edge(QUAD_EDGE e) {
  e = (QUAD_EDGE) ((uptrint) e ^ ((uptrint) e & ANDF));
  onext(e) = avail_edge;
  avail_edge = e;
}



/****************************************************************/
/*	Geometric primitives                                    */
/****************************************************************/

BOOLEAN incircle(a,b,c,d)
     /* incircle, as in the Guibas-Stolfi paper. */
     VERTEX_PTR a,b,c,d;
{
  double adx, ady, bdx, bdy, cdx, cdy, dx, dy, anorm, bnorm, cnorm, dnorm;
  double dret ;
  VERTEX_PTR loc_a,loc_b,loc_c,loc_d;

  /*if (flag) printf("incircle: 0x%x,0x%x,0x%x,0x%x\n",a,b,c,d);*/
  loc_d = d; 
  dx = X(loc_d); dy = Y(loc_d); dnorm = NORM(loc_d); 
  loc_a = a;
  adx = X(loc_a) - dx; ady = Y(loc_a) - dy; anorm = NORM(loc_a);  /* <--- 4% load penalty */
  loc_b = b;
  bdx = X(loc_b) - dx;  bdy = Y(loc_b) - dy; bnorm = NORM(loc_b);  /* <--- 5% load penalty */
  loc_c = c;
  cdx = X(loc_c) - dx;  cdy = Y(loc_c) - dy; cnorm = NORM(loc_c); 
  /*if (flag) printf("adx=%f,bdx=%f,cdx=%f\n",adx,bdx,cdx);*/
  dret =  (anorm - dnorm) * (bdx * cdy - bdy * cdx);
  dret += (bnorm - dnorm) * (cdx * ady - cdy * adx);
  dret += (cnorm - dnorm) * (adx * bdy - ady * bdx);
  /*if (flag) printf("Incircle: %f\n", dret);*/
  return 0.0 < dret;
}

/* TRUE iff A, B, C form a counterclockwise oriented triangle */
BOOLEAN ccw(VERTEX_PTR a, VERTEX_PTR b, VERTEX_PTR c) {
  double dret ;
  double xa,ya,xb,yb,xc,yc;
  VERTEX_PTR loc_a,loc_b,loc_c;
  loc_a = a;
  xa=X(loc_a); ya=Y(loc_a);
  loc_b = b;
  xb=X(loc_b); yb=Y(loc_b);             /*<----------------------------- 10 % load penalty */
  loc_c = c;
  xc=X(loc_c); yc=Y(loc_c);
  /*if (flag) printf("CCW:xa=%f,xb=%f,xd=%f\n",xa,xb,xc);*/
  /*if (flag) printf("CCW:ya=%f,yb=%f,yd=%f\n",ya,yb,yc);*/
  dret = (xa-xc)*(yb-yc) - (xb-xc)*(ya-yc);
  /*if (flag) printf("ccw: %f\n", dret);*/
  return dret > 0.0;
}

/****************************************************************/
/*	Quad-edge manipulation primitives                       */
/****************************************************************/
QUAD_EDGE makeedge(origin, destination)
VERTEX_PTR origin, destination;
{
    register QUAD_EDGE temp, ans;
    temp =  alloc_edge();
    ans = temp;

    onext(temp) = ans;
    orig(temp) = origin;
    temp = (QUAD_EDGE) ((uptrint) temp+SIZE);
    onext(temp) = (QUAD_EDGE) ((uptrint) ans + 3*SIZE);
    temp = (QUAD_EDGE) ((uptrint) temp+SIZE);
    onext(temp) = (QUAD_EDGE) ((uptrint) ans + 2*SIZE);
    orig(temp) = destination;
    temp = (QUAD_EDGE) ((uptrint) temp+SIZE);
    onext(temp) = (QUAD_EDGE) ((uptrint) ans + 1*SIZE);

    /*printf("Edge made @ 0x%x\n",ans);*/
    /*dump_quad(ans);*/
    return(ans);
}

void splice(a,b)
QUAD_EDGE a, b;
{
    QUAD_EDGE alpha, beta, temp;
    QUAD_EDGE t1;

    /*printf("begin splice 0x%x,0x%x\n",a,b);*/
    /*dump_quad(a); dump_quad(b);*/
    alpha = rot(onext(a));
    beta = rot(onext(b));  /*<---------------------------------- 15% load miss penalty  */
     /*dump_quad(alpha); dump_quad(beta);*/
    t1 = onext(beta);      /*<---------------------------------- 3%  load miss penalty  */
    temp = onext(alpha);   /*<---------------------------------- 11% load miss penalty  */
    onext(alpha) = t1;  
    /*printf("Writing 0x%x at onext of 0x%x\n",t1,alpha);*/

    onext(beta) = temp;
    /*printf("Writing 0x%x at onext of 0x%x\n",temp,beta);*/
    temp = onext(a); 
    t1 = onext(b); 
    onext(b) = temp;       
    onext(a) = t1;
    /*printf("Wrote 0x%x at onext of 0x%x\n",temp,b);*/
    /*printf("Wrote 0x%x at onext of 0x%x\n",t1,a);*/
    /*printf("End splice\n");*/
}

void swapedge(e)
QUAD_EDGE e;
{
    QUAD_EDGE a,b,syme,lnexttmp;
    VERTEX_PTR a1,b1;
    
    /*printf("begin swapedge\n");*/
    a = oprev(e);
    syme = sym(e);
    b = oprev(syme); 
    splice(e, a);
    splice(syme, b);
    lnexttmp = lnext(a);
    splice(e, lnexttmp);
    lnexttmp = lnext(b);
    splice(syme, lnexttmp);
    a1 = dest(a);
    b1 = dest(b);
    orig(e) = a1;
    dest(e) = b1; 
    /*printf("end swapedge\n");*/
}

/****************************************************************/
/*	The Merge Procedure. */
/****************************************************************/
/*#define valid(l) ccw(orig(basel), dest(l), dest(basel))*/

int valid(l,basel)
     QUAD_EDGE l,basel;
{
  register VERTEX_PTR t1,t2,t3;

  /*printf("valid:0x%x,0x%x\n",l,basel);*/

  t1=orig(basel);
  t3=dest(basel);

  t2=dest(l);   /* <------------------------------------------------- 11 % load penalty */
  return ccw(t1,t2,t3);
}

void dump_quad(ptr)
     QUAD_EDGE ptr;
{
  int i;
  QUAD_EDGE j;
  VERTEX_PTR v;

  ptr = (QUAD_EDGE) ((uptrint) ptr & ~ANDF);
  printf("Entered DUMP_QUAD: ptr=0x%p\n",ptr);
  for (i=0; i<4; i++)
   {
    j=onext(((QUAD_EDGE) (ptr+i)));
    v = orig(j);
    printf("DUMP_QUAD: ptr=0x%p onext=0x%p,v=0x%p\n",ptr+i,j,v);
   }
}






EDGE_PAIR do_merge(QUAD_EDGE ldo, QUAD_EDGE ldi, QUAD_EDGE rdi, QUAD_EDGE rdo)
{
  int rvalid, lvalid;
  register QUAD_EDGE basel,lcand,rcand,t;
  VERTEX_PTR t1,t2;

/*printf("merge\n");*/
  while (1) {
    VERTEX_PTR t3=orig(rdi);
    /*loc=rdi;*/
    /*t3=orig(loc);*/

    t1=orig(ldi);
    t2=dest(ldi);
    /*loc = ldi;*/
    /*t1 = orig(loc);*/
    /*t2 = dest(loc);*/

    while (ccw(t1,t2,t3/*orig(ldi), dest(ldi), orig(rdi)*/)) 
      {
	  ldi = lnext(ldi);
      /*ldi = lnext(loc);*/
      /*loc = ldi;*/

	   t1=orig(ldi);
	   t2=dest(ldi);
      /*t1 = orig(loc);*/
      /*t2 = dest(loc);*/
      }
    /*loc = rdi;*/

    t2=dest(rdi);
    /*t2 = dest(loc);*/

    if (ccw(t2,t3,t1/*dest(rdi), orig(rdi), orig(ldi)*/)) 
      {  rdi = rprev(rdi); }
      /*{ rdi = rprev(loc); }*/
    else { break; }
  }
  
  basel = connect_left(sym(rdi), ldi);

  /*loc = basel;*/

  lcand = rprev(basel);
  rcand = oprev(basel);
  t1 = orig(basel);
  t2 = dest(basel);
  /*lcand = rprev(loc);*/
  /*rcand = oprev(loc);*/
  /*t1 = orig(loc);*/
  /*t2 = dest(loc);*/

  if (t1/*orig(basel)*/ == orig(rdo)) rdo = basel;
  if (t2/*dest(basel)*/ == orig(ldo)) ldo = sym(basel);
  
  while (1) {
    VERTEX_PTR v1,v2,v3,v4;

    /*printf("valid site 1,lcand=0x%x,basel=0x%x\n",lcand,basel);*/
    /*dump_quad(lcand);*/
    t=onext(lcand);
    if (valid(t,basel)) {
      v4=orig(basel);

      /*loc = lcand;*/
      v1=dest(lcand);
      v3=orig(lcand);
      /*v1=dest(loc);*/
      /*v3=orig(loc);*/

      v2=dest(t);
      while (incircle(v1,v2,v3,v4
		      /*dest(lcand), dest(t), orig(lcand), orig(basel)*/))
	{
	  deleteedge(lcand);
	  lcand = t;

     /*loc = lcand;*/
	  t = onext(lcand);
	  v1=dest(lcand);
	  v3=orig(lcand);
	  /*t = onext(loc);*/
	  /*v1=dest(loc);*/
	  /*v3=orig(loc);*/

	  v2=dest(t);         /* <---- 3% load penalty */
	}
    }

    /*printf("valid site 2\n");*/
    t=oprev(rcand);
    if (valid(t,basel)) {
      v4=dest(basel);
      v1=dest(t);
      v2=dest(rcand);
      v3=orig(rcand);
      while (incircle(v1,v2,v3,v4
		      /*dest(t), dest(rcand), orig(rcand), dest(basel)*/))
	{
	  deleteedge(rcand);
	  rcand = t;
	  t = oprev(rcand);
	  v2=dest(rcand);
	  v3=orig(rcand);
	  v1=dest(t);                              /* <--- 4% load penalty */
	}
    }


    /*printf("Valid sites 3,4\n");*/
    lvalid = valid(lcand,basel);
    /*printf("Valid sites 3,4\n");*/
    rvalid = valid(rcand,basel);

    if ((! lvalid) && (! rvalid))
      {
	EDGE_PAIR retval; 
	retval.left = ldo; retval.right = rdo; return retval;
      }

    v1=dest(lcand);
    v2=orig(lcand);
    v3=orig(rcand);
    v4=dest(rcand);

    if (!lvalid ||
	(rvalid && incircle(v1,v2,v3,v4
			    /*dest(lcand), orig(lcand),
			    orig(rcand), dest(rcand)*/)))
      {
	basel = connect_left(rcand, sym(basel));
	rcand = lnext(sym(basel));
      }
    else
      {
	basel = sym(connect_right(lcand, basel));
	lcand = rprev(basel);
      }
  }
}


#define CONST_m1 10000
#define CONST_b 31415821
#define RANGE 100

int loop = 0, randum = 1, filein = 0 , fileout = 1, statistics = 0; 

void in_order(tree)
     VERTEX_PTR tree;
{
  VERTEX_PTR tleft, tright;
  double x, y;

  if (!tree) {
    printf("NULL\n");
    return;
  }

  x = X(tree);
  y = Y(tree);
  printf("X=%f, Y=%f\n",x, y);
  tleft = tree->left;
  in_order(tleft);
  tright = tree->right;
  in_order(tright);
}

int mult(int p, int q)
{
	int p1, p0, q1, q0;
	
	p1=p/CONST_m1; p0=p%CONST_m1;
	q1=q/CONST_m1; q0=q%CONST_m1;
	return (((p0*q1+p1*q0) % CONST_m1)*CONST_m1+p0*q0);
}

int skiprand(int seed, int n)
/* Generate the nth random # */
{
  for (; n; n--) seed=myrandom(seed);
  return seed;
}

int myrandom(int seed)
{
  seed = (mult(seed,CONST_b)+1);
  return seed;
}



void print_extra(VERTEX_PTR extra) {
  double x, y;
  x = X(extra);
  y = Y(extra);
  printf("X=%f, Y=%f\n",x, y);
}

int main(int argc, char **argv) {
  struct EDGE_STACK *my_stack;
  struct get_point point, extra;
  QUAD_EDGE edge;
  int n, retained;
  to_lincoln = to_off = to_3d_out = to_color = 0;
  voronoi = delaunay = 1; interactive = ahost = 0 ;
  retained = 0;
  
  printf("argc = %d\n",argc);
  n = dealwithargs(argc, argv);

/*  delete_all_edges();*/
  if (1) {
    printf("getting %d points\n", n);
    extra=get_points(1,1.0,n,1023,NumNodes-1,1);
    point=get_points(n-1,extra.curmax,n-1,extra.seed,0,NumNodes);
    printf("Done getting points\n");
    num_vertices = n + 1;
    my_stack=allocate_stack(num_vertices);
    if (flag) in_order(point.v);
    if (flag) print_extra(extra.v);
    printf("Doing voronoi on %d nodes\n", n); 

    edge=build_delaunay_triangulation(point.v,extra.v);
    
    if (flag) output_voronoi_diagram(edge,n,my_stack);
  }
  /*  delete_all_edges();*/
  return 0;
}

struct EDGE_STACK *allocate_stack(int num_vertice)
{
  struct EDGE_STACK *my_stack;

  num_edgeparts = 12*num_vertice;
  my_stack = (struct EDGE_STACK *)malloc(sizeof(struct EDGE_STACK));
  my_stack->elts = (QUAD_EDGE *)malloc(num_edgeparts * sizeof(QUAD_EDGE));
  my_stack->stack_size = num_edgeparts/2;
  return my_stack;
}

void free_all(cont,my_stack)
     int cont;
     struct EDGE_STACK *my_stack;
{
  free(my_stack->elts);
  free(my_stack);
}

struct get_point get_points(int n, double curmax,int i, int seed,
                            int processor, int numnodes)
{
  VERTEX_PTR node;
  struct get_point point;
  int j;

  //printf("seed = %d %f %d %d %d %d\n", n, curmax, i, seed, processor, numnodes);

  if (n<1) {
    point.v = NULL;
    point.curmax=curmax;
	 point.seed = seed;
    return point;
  }
  /*printf("Get points: %d, %f, %d\n",n,curmax,processor);*/
  point = get_points(n/2,curmax,i,seed,processor+numnodes/2,numnodes/2);
  /*printf("rec call n=%d\n",n);*/
  i -= n/2;

  j = NumNodes - ((NumNodes-1)/2);

  node = (VERTEX_PTR) malloc(sizeof(struct VERTEX));

  /*printf("Get points past alloc,n=%d\n",n);*/
  //printf("%f\n", (double)point.seed);
  X(node) = point.curmax * exp(log(drand(point.seed))/i);
  Y(node) = drand(point.seed);
  NORM(node) = X(node)*X(node) + Y(node)*Y(node);
  node->right = point.v;
  /*printf("node = 0x%x\n",node);*/
  point = get_points(n/2,X(node),i-1,point.seed,
		     processor,numnodes/2);
  node->left = point.v;
  point.v = node;
  return point;
}


/****************************************************************/
/*	Voronoi Diagram Routines. */
/****************************************************************/

/****************************************************************/
/*	Graph Traversal Routines */
/****************************************************************/

QUAD_EDGE pop_edge(struct EDGE_STACK *x) {
  int a=x->ptr--;
  return (x)->elts[a];
}

void push_edge(struct EDGE_STACK *stack, QUAD_EDGE edge) {
  register int a;
  /*printf("pushing edge \n");*/
  if (stack->ptr == stack->stack_size) {
    printf("cannot push onto stack: stack is too large\n");
  }
  else {
    a = stack->ptr;
    a++;
    stack->ptr = a;
    stack->elts[a] = edge;
  }
}

void push_ring(struct EDGE_STACK *stack, QUAD_EDGE edge) {
    QUAD_EDGE nex;
    nex = onext(edge);
    while (nex != edge) {
	if (seen(nex) == 0) {
	   seen(nex) = 1;
	   push_edge(stack, nex);
	}
	nex = onext(nex);
    }
}

void push_nonzero_ring(stack, edge)
     struct EDGE_STACK *stack;
     QUAD_EDGE edge;
{
  QUAD_EDGE nex;
  nex = onext(edge);
  while (nex != edge) {
    if (seen(nex) != 0) {
      seen(nex) = 0;
      push_edge(stack, nex);
    }
    nex = onext(nex);
  }
}

void zero_seen(my_stack,edge)
QUAD_EDGE edge;
struct EDGE_STACK *my_stack;
{
  my_stack->ptr = 0;
  push_nonzero_ring(my_stack, edge);
  while (my_stack->ptr != 0) {
    edge = pop_edge(my_stack);
    push_nonzero_ring(my_stack, sym(edge));
  }
}
