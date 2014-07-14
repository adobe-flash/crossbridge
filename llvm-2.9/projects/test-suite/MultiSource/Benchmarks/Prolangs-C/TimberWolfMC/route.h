#include "port.h"

#define VLARGE 1000000000

typedef struct chanbox {
    int from ;
    int to ;
    int foffset ;
    int toffset ;
    struct chanbox *next ;
} CHANBOX, *CHANBOXPTR ;

typedef struct altbox {
    int *pinList ;
    int distance ;
    CHANBOXPTR chanList ;
} ALTBOX , *ALTBOXPTR ;

typedef struct nrbox {
    ALTBOXPTR *alternate ;
    int numRoutes ;
    int currentRoute ;
    int newRoute ;
} NRBOX , *NRBOXPTR ;

typedef struct gnode {
    int node ;
    int ilength ;
    int length ;
    int cost ;
    int capacity ;
    int inactive ;
    int einactive ;
    struct gnode *next ;
} GNODE , *GNODEPTR ;

typedef struct nnode {
    int temp ;
    int distance ;
    short int from ;
    short int from2 ;
} NNODE , *NNODEPTR ;

typedef struct list2 {
    int node ;
    struct list2 *next ;
} LIST2 , *LIST2PTR ;

typedef struct pnode {
    int eptr ;
    NNODEPTR nodeList ;
    LIST2PTR equiv ;
} PNODE , *PNODEPTR ;

typedef struct quad {
    int PorE ;
    int node1 ;
    int node2 ;
    int distance ;
    struct quad *next ;
    struct quad *prev ;
} QUAD, *QUADPTR ;

typedef struct path {
    short int *nodeset ;
    int p ;
    int q ; 
    int excluded ;
    int exlist[5] ;
} PATH, *PATHPTR ;

extern short int *tempArray ;
extern GNODEPTR *gnodeArray ;
extern PNODEPTR pnodeArray ;
extern int numnodes ;
extern int numpins ;
extern char *cktName ;
extern int maxpnode ;
extern char **pnameArray ;
extern char **nnameArray ;
extern int *pinOffset ;
extern QUADPTR pinlist ;
extern FILE *fpo ;
extern int **pathList ;
extern short int **netSegArray ;
extern short int *segList ;
extern int segPtr ;
extern short int *sourceList ;
extern int sourcePtr ;
extern short int *targetList ;
extern int targetPtr ;
extern int Mpaths ;
extern short int *delSourceList ;
extern short int *addTargetList ;
extern int pathLength ;
extern short int **savePaths ;
extern int largestNet ;
extern NRBOXPTR netRoutes ;
extern short int **gtrace ;
extern PATHPTR pathArray ;
extern int MAXPATHS ;
extern int EXTRASOURCES ;
