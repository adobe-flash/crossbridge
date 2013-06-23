#include "port.h"
#include "23tree.h"
#define VBIG 10000000
#define ABS(value)   ( (value)>=0 ? (value) : -(value) ) 

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

extern CLBOXPTR *cellList ;
extern int pitch ;
extern BELLBOXPTR xBellArray , yBellArray ;
extern int **eIndexArray ;
extern WCPTR *xNodeArray , *yNodeArray ;
extern NODPTR xNodules , yNodules ;
extern int numXnodes , numYnodes ;
extern int numberCells ;
extern int edgeTransition ;
extern char *cktName ;
extern TNODEPTR LEroot , BEroot ;
extern TNODEPTR VDroot , HRroot ;
extern TNODEPTR Vroot , Hroot ;
extern TNODEPTR vChanEndRoot , vChanBeginRoot ;
extern TNODEPTR hChanEndRoot , hChanBeginRoot ;
extern DLINK2PTR *LEptrs , *BEptrs ;
extern DLINK1PTR *VDptrs , *HRptrs ;
extern DLINK1PTR *Vptrs , *Hptrs ;
extern EBOXPTR eArray ;
extern int eNum ;
extern RECTPTR rectArray ;
extern int numRects ;
extern DLINK1PTR Hlist , Vlist ;
extern DLINK2PTR HRlist , VRlist ;
extern DLINK1PTR Hend , Vend ;
extern DLINK1PTR hFixedList , vFixedList ;
extern DLINK1PTR hFixedEnd , vFixedEnd ;
extern EDGEBOXPTR edgeList ;
extern int edgeCount ;
extern int numProbes ;
extern TNODEPTR hFixedEdgeRoot ;
extern TNODEPTR vFixedEdgeRoot ;
extern TNODEPTR hEdgeRoot , vEdgeRoot ;
extern TNODEPTR hRectRoot , vRectRoot ;
extern int bbl , bbr , bbb , bbt ;
extern FILE *fpdebug ;
