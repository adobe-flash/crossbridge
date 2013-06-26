#include "port.h"

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

short int *tempArray ;
GNODEPTR *gnodeArray ;
PNODEPTR pnodeArray ;
int numnodes ;
int numpins ;
int maxpnode ;
char **pnameArray ;
char **nnameArray ;
int *pinOffset ;
QUADPTR pinlist ;
int **pathList ;
short int **netSegArray ;
short int *segList ;
int segPtr ;
short int *sourceList ;
int sourcePtr ;
short int *targetList ;
int targetPtr ;
int Mpaths ;
short int *delSourceList ;
short int *addTargetList ;
int pathLength ;
short int **savePaths ;
int largestNet ;
NRBOXPTR netRoutes ;
short int **gtrace ;
PATHPTR pathArray ;
int MAXPATHS ;
int EXTRASOURCES;
