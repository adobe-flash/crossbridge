#include "port.h"

#define ZERO 0
#define ALLOC(type)    (  (type *) malloc ( sizeof(type) )  )
#define LINE printf(" file : %s  line # %d\n", __FILE__,__LINE__);
#define ABS(value)   ( (value)>=0 ? (value) : -(value) ) 

#define CELLNULL (CELLBOXPTR) NULL
#define NETNULL (NETBOXPTR) NULL
#define DIMNULL (DIMBOXPTR) NULL
#define TILENULL (TILEBOXPTR) NULL
#define TERMNULL (TERMBOXPTR) NULL
#define TERMNETSNULL (TERMNETSPTR) NULL
#define LNULL (LOCBOX *) NULL

typedef struct bustbox {
    int xc ;
    int yc ;
} BUSTBOX , *BUSTBOXPTR ;

typedef struct locbox {
    int xpos ;
    int ypos ;
    int oxpos ;
    int oypos ;
} LOCBOX ;

typedef struct contentbox {
    int contents ;
    int newContents ;
    int capacity ;
    int HorV ;
    int span ;
} CONTENTBOX ;

typedef struct uncombox {
    int terminal  ;
    int site      ;
    int newsite   ;
    int numLocs   ;
    int numranges ;
    int range1    ;
    int range2    ;
    int sequence  ;
    int groupflag ;
    int finalx    ;
    int finaly    ;
} UNCOMBOX ;

typedef struct netbox {
    struct netbox *nextterm ;
    int xpos ;
    int ypos ;
    int newx ;
    int newy ;
    int terminal ;
    int flag ;
    int cell ;
    int PorE ;
    int skip ;
} *NETBOXPTR , NETBOX ;

typedef struct termbox { 
    struct termbox *nextterm ;
    int xpos     ;
    int ypos     ;
    int oxpos     ;
    int oypos     ;
    int terminal ;
} *TERMBOXPTR , TERMBOX ;

typedef struct tilebox { 
    struct tilebox *nexttile ;
    double lweight;
    double rweight;
    double bweight;
    double tweight;
    int lborder   ;
    int rborder   ;
    int bborder   ;
    int tborder   ;
    int left      ;
    int right     ;
    int bottom    ;
    int top       ;
    int oleft     ;
    int oright    ;
    int obottom   ;
    int otop      ;
    TERMBOXPTR termptr ;
    LOCBOX *siteLocArray ;
} *TILEBOXPTR , TILEBOX ;

typedef struct psidebox {
    int length   ;
    double pincount;
    int vertical ;
    int position ;
} PSIDEBOX ;

typedef struct sidebox {
    int firstSite ;
    int lastSite  ;
} SIDEBOX ;

typedef struct cellbox { 
    char *cname             ;
    int class               ;
    int xcenter             ;
    int ycenter             ;
    int orientList[9]       ;
    int orient              ;
    int numtiles            ;
    int numsides            ;
    int numterms            ;
    int numgroups           ;
    int softflag            ;
    int padside             ;
    double sidespace        ;
    double aspect           ;
    double aspectO          ;
    double aspUB            ;
    double aspLB            ;
    int numsites            ;
    int numUnComTerms       ;
    CONTENTBOX *siteContent ;
    UNCOMBOX *unComTerms    ;
    TILEBOXPTR config[8]    ;
    SIDEBOX *sideArray      ;
} *CELLBOXPTR , CELLBOX ;

typedef struct dimbox {
    NETBOXPTR netptr   ;
    int skip       ;
    int xmin       ;
    int newxmin    ;
    int xmax       ;
    int newxmax    ;
    int ymin       ;
    int flag       ;
    int newymin    ;
    int ymax       ;
    int newymax    ;
    double Hweight ;
    double Vweight ;
    char *nname    ;
} *DIMBOXPTR , DIMBOX ;

typedef struct termnets {
    int net ;
    NETBOXPTR termptr ;
} *TERMNETSPTR , TERMNETS ;

typedef struct hash {
    char *hname ;
    int hnum ;
    struct hash *hnext ;
} HASHBOX , *HASHPTR ;

CELLBOXPTR *cellarray   ;
DIMBOXPTR *netarray     ;
TERMNETSPTR *termarray  ;
int ***blockarray ;

int bdxlength , bdylength ;
int blockl , blockr , blockt , blockb ;
int blockmx , blockmy ;

int numcells  ;
int numpads   ;
int numBinsX  ;
int numBinsY  ;
int binWidthX ;
int binWidthY ;
int binOffsetX;
int binOffsetY;
int *bucket   ;
int maxWeight ;
int baseWeight;
double slopeX    ;
double slopeY    ;
double basefactor;
int aveChanWid;

int penalty   ;
int funccost  ;
int offset    ;
int overfill  ;

HASHPTR *hashtab ;
int netctr ;
double T   ;
int randVar ;
FILE *fpo ;
char **pinnames ;
char *cktName ;
int maxterm ;
int numnets ;
int attmax  ;

int *occa1ptr ;
int *occa2ptr ;
int *occb1ptr ;
int *occb2ptr ;
int binX   ; 
int binY   ; 

double pinsPerLen ;
PSIDEBOX *pSideArray ;
int trackspacing ;
int pinSpacing   ;
double lapFactor ;

int (*overlap)() ;
int (*overlapf)() ;
int (*overlapx)() ;
int goverlap() ;
int goverlapf() ;
int goverlapx() ;
int woverlap() ;
int woverlapf() ;
int woverlapx() ;

int gOffsetX ;
int gOffsetY ;
int gridX ;
int gridY ;
int coreGiven ;
int gridGiven ;
int layersFactor ;
