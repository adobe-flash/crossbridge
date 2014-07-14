/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <math.h>

typedef int BOOLEAN;
typedef unsigned long      uptrint;

struct edge_rec {
  struct VERTEX *v;
  struct edge_rec *next;
  long wasseen;
  void *Buffer;
};


struct get_point {
  struct VERTEX *v;
  double curmax;
  int seed;
};


typedef struct edge_rec *EDGE_PTR;
typedef struct VERTEX *VERTEX_PTR;
typedef struct edge_rec *QUAD_EDGE;
  
struct VEC2 {
  double x,y;
  double norm;
};


struct VERTEX {
  struct VEC2 v;
  struct VERTEX *left, *right;
};


typedef struct {
  QUAD_EDGE left, right;
} EDGE_PAIR;


struct EDGE_STACK {
    int ptr;
    QUAD_EDGE *elts ;
    int stack_size;
};


#define onext(a) (a)->next
#define oprev(a) rot(onext(rot(a)))
#define lnext(a) rot(onext(rotinv(a)))
#define lprev(a) sym(onext(a))
#define rnext(a) rotinv(onext(rot(a)))
#define rprev(a) onext(sym(a))
#define dnext(a) sym(onext(sym(a)))
#define dprev(a) rotinv(onext(rotinv(a)))

#define X(r) r->v.x
#define Y(r) r->v.y
#define NORM(r) r->v.norm
#define orig(a) (a)->v
#define dest(a) orig(sym(a))
#define seen(a) (a)->wasseen
  
#define origv(a) orig(a)->v
#define destv(a) dest(a)->v


#define SIZE ((uptrint) sizeof(struct edge_rec))
#define ANDF ((uptrint) 4*sizeof(struct edge_rec) - 1)

#define sym(a) ((QUAD_EDGE) (((uptrint) (a)) ^ 2*SIZE))
#define rot(a) ((QUAD_EDGE) ( (((uptrint) (a) + 1*SIZE) & ANDF) | ((uptrint) (a) & ~ANDF) ))
#define rotinv(a) ((QUAD_EDGE) ( (((uptrint) (a) + 3*SIZE) & ANDF) | ((uptrint) (a) & ~ANDF) ))
#define base(a) ((QUAD_EDGE) ((uptrint a) & ~ANDF))

QUAD_EDGE alloc_edge();
void free_edge(QUAD_EDGE e);
QUAD_EDGE makeedge();
void splice();
void swapedge();
void deleteedge();
QUAD_EDGE build_delaunay_triangulation();
EDGE_PAIR build_delaunay(VERTEX_PTR tree, VERTEX_PTR extra);
EDGE_PAIR do_merge(QUAD_EDGE ldo, QUAD_EDGE ldi, QUAD_EDGE rdi, QUAD_EDGE rdo);
QUAD_EDGE connect_left();
QUAD_EDGE connect_right();

int myrandom(int seed);
void zero_seen();
QUAD_EDGE pop_edge();

#define drand(seed) (((double) (seed=myrandom(seed))) / (double) 2147483647)

extern VERTEX_PTR *vp ;
extern struct VERTEX *va ;
extern EDGE_PTR *next ;
extern VERTEX_PTR *org ;
extern int num_vertices, num_edgeparts, stack_size ;
extern int to_lincoln , to_off, to_3d_out, to_color , voronoi , delaunay , interactive , ahost ;
extern char *see;
extern int NumNodes, NDim;


void push_ring(struct EDGE_STACK *stack, QUAD_EDGE edge);
void push_edge(struct EDGE_STACK *stack, QUAD_EDGE edge);
BOOLEAN ccw(VERTEX_PTR a, VERTEX_PTR b, VERTEX_PTR c);
int dealwithargs(int argc, char *argv[]);
void output_voronoi_diagram(QUAD_EDGE edge, int nv, struct EDGE_STACK *stack);
struct get_point get_points(int n, double curmax,int i, int seed,
                            int processor, int numnodes);
struct EDGE_STACK *allocate_stack(int num_vertices);


double V2_cprod(struct VEC2 u,struct VEC2 v);
double V2_dot(struct VEC2 u, struct VEC2 v);
struct VEC2 V2_times(double c, struct VEC2 v);
struct VEC2 V2_sum(struct VEC2 u, struct VEC2 v);
struct VEC2 V2_sub(struct VEC2 u, struct VEC2 v);
double V2_magn(struct VEC2 u);
struct VEC2 V2_cross(struct VEC2 v);
