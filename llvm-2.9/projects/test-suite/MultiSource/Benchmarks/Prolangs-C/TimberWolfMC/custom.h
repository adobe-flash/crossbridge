#include "port.h"

#define MaxSites 50
#define ZERO 0
#define ALLOC(type)    (  (type *) malloc ( sizeof(type) )  )
#define LINE printf(" file : %s  line # %d\n", __FILE__,__LINE__);
#define ABS(value)   ( (value)>=0 ? (value) : -(value) ) 
#define ROUND(value) ( ( (value) - (int)(value) ) >= 0.5 ? ((int)(value) + 1) : ((int)(value)) ) 
#define RAND ((randVar = randVar * 1103515245 + 12345) & 0x7fffffff)

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
    int oxpos    ;
    int oypos    ;
    int terminal ;
} *TERMBOXPTR , TERMBOX ;

typedef struct tilebox { 
    struct tilebox *nexttile ;
    double lweight ;
    double rweight ;
    double bweight ;
    double tweight ;
    int lborder    ;
    int rborder    ;
    int bborder    ;
    int tborder    ;
    int left       ;
    int right      ;
    int bottom     ;
    int top        ;
    int oleft      ;
    int oright     ;
    int obottom    ;
    int otop       ;
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
    int skip     ;
    int xmin     ;
    int newxmin  ;
    int xmax     ;
    int newxmax  ;
    int ymin     ;
    int flag     ;
    int newymin  ;
    int ymax     ;
    int newymax  ;
    double Hweight ;
    double Vweight ;
    char *nname  ;
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

extern CELLBOXPTR *cellarray   ;
extern DIMBOXPTR *netarray     ;
extern TERMNETSPTR *termarray  ;
extern int ***blockarray ;

extern int bdxlength , bdylength ;
extern int blockl , blockr , blockt , blockb ;
extern int blockmx , blockmy ;

extern int numcells  ;
extern int numpads   ;
extern int numBinsX  ;
extern int numBinsY  ;
extern int binWidthX ;
extern int binWidthY ;
extern int binOffsetX;
extern int binOffsetY;
extern int *bucket   ;
extern int maxWeight ;
extern int baseWeight;
extern double slopeX    ;
extern double slopeY    ;
extern double basefactor;
extern int aveChanWid;

extern int penalty   ;
extern int funccost  ;
extern int offset    ;
extern int overfill  ;

extern HASHPTR *hashtab ;
extern int netctr ;
extern double T ;
extern int randVar ;
extern FILE *fpo ;
extern char **pinnames ;
extern char *cktName ;
extern int numnets ;
extern int maxterm ;
extern int attmax  ;

extern int *occa1ptr ;
extern int *occa2ptr ;
extern int *occb1ptr ;
extern int *occb2ptr ;
extern int binX   ; 
extern int binY   ; 

extern double pinsPerLen ;
extern PSIDEBOX *pSideArray ;
extern int trackspacing ;
extern int pinSpacing   ;
extern double lapFactor ;
extern int (*overlap)() ;
extern int (*overlapf)() ;
extern int (*overlapx)() ;
extern int goverlap() ;
extern int goverlapf() ;
extern int goverlapx() ;
extern int woverlap() ;
extern int woverlapf() ;
extern int woverlapx() ;

extern int gOffsetX ;
extern int gOffsetY ;
extern int gridX ;
extern int gridY ;
extern int coreGiven ;
extern int gridGiven ;
extern int layersFactor ;
