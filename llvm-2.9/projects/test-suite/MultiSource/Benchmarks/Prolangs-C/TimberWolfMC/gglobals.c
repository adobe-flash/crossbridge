#include "port.h"
#include "23tree.h"

typedef struct edgebox {
    int cell   ;
    int start  ;
    int end    ;
    int loc    ;
    int length ;
    int UorR   ;
    int fixed  ;
    int prevEdge ;
    int nextEdge ;
    int node     ;
} EDGEBOX, *EDGEBOXPTR ;

typedef struct dlink1 {
    int edge ;
    struct dlink1 *prev ;
    struct dlink1 *next ;
} DLINK1 , *DLINK1PTR ;

typedef struct dlink2 {
    int index  ;
    struct dlink2 *prev ;
    struct dlink2 *next ;
} DLINK2 , *DLINK2PTR ;

typedef struct rect {
    int xc ;
    int yc ;
    int xreset ;
    int yreset ;
    int l ;
    int r ;
    int b ;
    int t ;
    int ur ;
    int ul ;
    int lr ;
    int ll ;
    int nedges ;
} RECT , *RECTPTR ;

typedef struct ebox {
    int index1  ;
    int index2  ;
    int width   ;
    int lbside  ;
    int rtside  ;
    int length  ;
    int hiend   ;
    int loend   ;
    int edge1   ;
    int edge2   ;
    int density ;
    int notActive ;
    TNODEPTR root ;
} EBOX, *EBOXPTR ;

typedef struct bellbox {
    int distance ;
    int from ;
} BELLBOX, *BELLBOXPTR ;

typedef struct flare {
    int fixEdge ;
    int *eindex ;
    struct flare *next ;
} FLARE, *FLAREPTR ;

typedef struct nodbox {
    FLAREPTR inList ;
    FLAREPTR outList ;
    int done ;
} NODBOX, *NODPTR ;

typedef struct wcbox {
    int fixedWidth ;
    int node ;
    int *channels ;
    int length ;
    struct wcbox *next ;
} WCBOX, *WCPTR ;

typedef struct clbox {
    int edge  ;
    int extraSpace ;
    int HorV ;
    int loc ;
    int start ;
    int end ;
    int compact ;
    struct clbox *next ;
} CLBOX, *CLBOXPTR ;

CLBOXPTR *cellList ;
int pitch ;
BELLBOXPTR xBellArray , yBellArray ;
int **eIndexArray ;
WCPTR *xNodeArray , *yNodeArray ;
NODPTR xNodules , yNodules ;
int numXnodes , numYnodes ;
int numberCells ;
int edgeTransition ;
char *cktName ;
TNODEPTR VDroot , HRroot ;
TNODEPTR LEroot , BEroot ;
TNODEPTR Vroot , Hroot ;
TNODEPTR vChanEndRoot , vChanBeginRoot ;
TNODEPTR hChanEndRoot , hChanBeginRoot ;
DLINK2PTR *LEptrs , *BEptrs ;
DLINK1PTR *VDptrs , *HRptrs ;
DLINK1PTR *Vptrs , *Hptrs ;
EBOXPTR eArray ;
int eNum ;
RECTPTR rectArray ;
int numRects ;
DLINK1PTR Hlist , Vlist ;
DLINK2PTR HRlist , VRlist ;
DLINK1PTR Hend , Vend ;
DLINK1PTR hFixedList , vFixedList ;
DLINK1PTR hFixedEnd , vFixedEnd ;
EDGEBOXPTR edgeList ;
int edgeCount ;
int numProbes ;
TNODEPTR hFixedEdgeRoot ;
TNODEPTR vFixedEdgeRoot ;
TNODEPTR hEdgeRoot , vEdgeRoot ;
TNODEPTR hRectRoot , vRectRoot ;
int bbl , bbr , bbb , bbt ;
FILE *fpdebug ;
