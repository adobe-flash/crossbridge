typedef struct point{
  int x,y;
} point;

typedef struct dpoint {
  double x,y;
} dpoint;

typedef struct DCEL_segment {
  struct point        v1,v2;   /* v1 start point, v2 end point */ 
  struct DCEL_segment *p1,*p2; /* Counter clock wise around v1 & v2 */ 
  struct DCEL_segment *q1,*q2; /* Clockwise around v1 & v2 */
} DCEL_segment;

typedef struct Knode {
  point p;
  int e1,e2,e3;
} Knode;

typedef struct Enode {
  int v1,v2;
  int p1,p2,q1,q2;
  int f1,f2;
} Enode;

typedef struct splay_element {
  long key;
  point p;
} splay_element;

typedef struct splay_node {
  splay_element element;
  struct splay_node *father, *left, *right;
} splay_node;

typedef struct CHpoints {
  int number;
  point node;
  int v;
  struct CHpoints *next,*prev;
} CHpoints;

typedef struct key {
  double radius;
  double angle;
  int number;
} key;

typedef struct CHsplay_element {
  key key;
  CHpoints *point;
} CHsplay_element;

typedef struct CHsplay_node {
  CHsplay_element element;
  struct CHsplay_node *father, *left, *right;
} CHsplay_node;
