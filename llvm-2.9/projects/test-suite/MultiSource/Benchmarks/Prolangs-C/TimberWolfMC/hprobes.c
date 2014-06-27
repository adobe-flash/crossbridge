#include "geo.h"
#define DEBUG

extern int tprop( TNODEPTR r , int value );
extern void tinsert( TNODEPTR *root , int value , int property );

void doubleBack( DLINK1PTR dptr );

void hprobes(void)
{

DLINK1PTR downPtr , up1ptr , up2ptr , checkPtr , ptr ;
int dx , dy1 , dy2 , dedge , by1 , by2 , uedge , edge , check ;
int u1y2 , u1y1 , u1x , u2y2 , u2y1 , u2x ;

downPtr = vFixedList ;
for( ; downPtr != (DLINK1PTR) NULL ; downPtr = downPtr->next ) {
    dedge = downPtr->edge ;
    if( edgeList[dedge].UorR > 0 ) {
	continue ;
    }
    dx  = edgeList[dedge].loc   ;
    dy1 = edgeList[dedge].start ;
    dy2 = edgeList[dedge].end   ;
    
    if( edgeList[ edgeList[dedge].prevEdge ].UorR == 1 ) {
	up2ptr = Vptrs[ tprop( Vroot , dx ) ] ;
	for(; up2ptr != (DLINK1PTR) NULL ; up2ptr = up2ptr->next){
	    uedge = up2ptr->edge ;
	    if( edgeList[uedge].UorR < 0 ) {
		continue ;
	    }
	    u2x  = edgeList[uedge].loc   ;
	    u2y1 = edgeList[uedge].start ;
	    u2y2 = edgeList[uedge].end   ;
	    if( u2y2 < dy2 || u2y1 >= dy2 ) {
		continue ;
	    }
	    break ;
	}
    } else {
	up2ptr = (DLINK1PTR) NULL ;
    }
    if( edgeList[ edgeList[dedge].nextEdge ].UorR == -1 ) {
	up1ptr = Vptrs[ tprop( Vroot , dx ) ] ;
	for(; up1ptr != (DLINK1PTR) NULL ; up1ptr = up1ptr->next){
	    uedge = up1ptr->edge ;
	    if( edgeList[uedge].UorR < 0 ) {
		continue ;
	    }
	    u1x  = edgeList[uedge].loc   ;
	    u1y1 = edgeList[uedge].start ;
	    u1y2 = edgeList[uedge].end   ;
	    if( u1y2 <= dy1 || u1y1 > dy1 ) {
		continue ;
	    }
	    break ;
	}
    } else {
	up1ptr = (DLINK1PTR) NULL ;
    }
    if( up2ptr != (DLINK1PTR) NULL && up2ptr == up1ptr ) {
	check = 1 ;
	checkPtr = Vptrs[ tprop( Vroot , dx ) ] ;
	for(; checkPtr != (DLINK1PTR) NULL ;
				    checkPtr = checkPtr->next ) {
	    if( edgeList[ checkPtr->edge ].UorR < 0 ) {
		continue ;
	    }
	    if( edgeList[ checkPtr->edge ].loc >= u2x ) {
		break ;
	    }
	    if( edgeList[ checkPtr->edge ].start >= dy2 ||
			edgeList[ checkPtr->edge ].end <= dy1 ) {
		continue ;
	    }
	    check = 0 ;
	    break ;
	}
	if( check ) {
	    edgeList[++numProbes + edgeCount].start  = dx ;
	    edgeList[numProbes + edgeCount].end      = u2x  ;
	    edgeList[numProbes + edgeCount].loc      = dy2 ;
	    edgeList[numProbes + edgeCount].length   = u2x - dx ;
	    edgeList[numProbes + edgeCount].fixed    = 0 ;
	    edgeList[numProbes + edgeCount].cell     = 0 ;
	    edgeList[numProbes + edgeCount].UorR     = 1 ;
	    tinsert( &hEdgeRoot, dy2 , numProbes + edgeCount ) ;
#ifdef DEBUG
	    fprintf(fpdebug,"hprobe:%d  ", numProbes ) ;
	    fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
					dx , u2x , dy2 , 1 ) ;
#endif
	    edgeList[++numProbes + edgeCount].start  = dx ;
	    edgeList[numProbes + edgeCount].end      = u2x  ;
	    edgeList[numProbes + edgeCount].loc      = dy1 ;
	    edgeList[numProbes + edgeCount].length   = u2x - dx ;
	    edgeList[numProbes + edgeCount].fixed    = 0 ;
	    edgeList[numProbes + edgeCount].cell     = 0 ;
	    edgeList[numProbes + edgeCount].UorR     = -1 ;
	    tinsert( &hEdgeRoot, dy1 , numProbes + edgeCount ) ;
#ifdef DEBUG
	    fprintf(fpdebug,"hprobe:%d  ", numProbes ) ;
	    fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
					dx , u2x , dy1 , -1 ) ;
#endif
	} else {
	    doubleBack( downPtr ) ;	
	}
	continue ;
    }
    if( up2ptr != (DLINK1PTR) NULL &&
	    edgeList[ edgeList[up2ptr->edge].prevEdge ].UorR == -1 ) {
	ptr = Vptrs[ tprop( Vroot , u2x ) ] ;
	for( ptr = ptr->next; ptr != (DLINK1PTR) NULL; ptr = ptr->next){
	    if( edgeList[ptr->edge].loc > u2x ) {
		break ;
	    }
	}
	if( ptr == (DLINK1PTR) NULL ) {
	    ptr = vFixedEnd ;
	} else {
	    ptr = ptr->prev ;
	}
	for( ; ptr != (DLINK1PTR) NULL; ptr = ptr->prev){
	    edge = ptr->edge ;
	    if( edgeList[edge].UorR > 0 ) {
		continue ;
	    }
	    by1 = edgeList[edge].start ;
	    by2 = edgeList[edge].end   ;
	    if( by2 <= u2y1 || by1 > u2y1 ) {
		continue ;
	    }
	    break ;
	}
	if( downPtr == ptr ) {
	    check = 1 ;
	    checkPtr = Vptrs[ tprop( Vroot , dx ) ] ;
	    for(; checkPtr != (DLINK1PTR) NULL ;
				    checkPtr = checkPtr->next ) {
		if( edgeList[ checkPtr->edge ].UorR < 0 ) {
		    continue ;
		}
		if( edgeList[ checkPtr->edge ].loc >= u2x ) {
		    break ;
		}
		if( edgeList[ checkPtr->edge ].start >= dy2 ||
			edgeList[ checkPtr->edge ].end <= u2y1 ) {
		    continue ;
		}
		check = 0 ;
		break ;
	    }
	    if( check ) {
		edgeList[++numProbes + edgeCount].start  = dx ;
		edgeList[numProbes + edgeCount].end      = u2x  ;
		edgeList[numProbes + edgeCount].loc      = dy2 ;
		edgeList[numProbes + edgeCount].length   = u2x - dx ;
		edgeList[numProbes + edgeCount].fixed    = 0 ;
		edgeList[numProbes + edgeCount].cell     = 0 ;
		edgeList[numProbes + edgeCount].UorR     = 1 ;
		tinsert( &hEdgeRoot, dy2 , numProbes + edgeCount ) ;
#ifdef DEBUG
		fprintf(fpdebug,"hprobe:%d  ", numProbes ) ;
		fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
					dx , u2x , dy2 , 1 ) ;
#endif
		edgeList[++numProbes + edgeCount].start  = dx ;
		edgeList[numProbes + edgeCount].end      = u2x  ;
		edgeList[numProbes + edgeCount].loc      = u2y1 ;
		edgeList[numProbes + edgeCount].length   = u2x - dx ;
		edgeList[numProbes + edgeCount].fixed    = 0 ;
		edgeList[numProbes + edgeCount].cell     = 0 ;
		edgeList[numProbes + edgeCount].UorR     = -1 ;
		tinsert( &hEdgeRoot, u2y1 , numProbes + edgeCount ) ;
#ifdef DEBUG
		fprintf(fpdebug,"hprobe:%d  ", numProbes ) ;
		fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
				    dx , u2x , u2y1 , -1 ) ;
#endif
	    }
	}
    }
    if( up1ptr != (DLINK1PTR) NULL &&
	    edgeList[ edgeList[up1ptr->edge].nextEdge ].UorR == 1 ) {
	ptr = Vptrs[ tprop( Vroot , u1x ) ] ;
	for( ptr = ptr->next; ptr != (DLINK1PTR) NULL; ptr = ptr->next){
	    if( edgeList[ptr->edge].loc > u1x ) {
		break ;
	    }
	}
	if( ptr == (DLINK1PTR) NULL ) {
	    ptr = vFixedEnd ;
	} else {
	    ptr = ptr->prev ;
	}
	for( ; ptr != (DLINK1PTR) NULL; ptr = ptr->prev){
	    edge = ptr->edge ;
	    if( edgeList[edge].UorR > 0 ) {
		continue ;
	    }
	    by1 = edgeList[edge].start ;
	    by2 = edgeList[edge].end   ;
	    if( by2 < u1y2 || by1 >= u1y2 ) {
		continue ;
	    }
	    break ;
	}
	if( downPtr == ptr ) {
	    check = 1 ;
	    checkPtr = Vptrs[ tprop( Vroot , dx ) ] ;
	    for(; checkPtr != (DLINK1PTR) NULL ;
				    checkPtr = checkPtr->next ) {
		if( edgeList[ checkPtr->edge ].UorR < 0 ) {
		    continue ;
		}
		if( edgeList[ checkPtr->edge ].loc >= u1x ) {
		    break ;
		}
		if( edgeList[ checkPtr->edge ].start >= u1y2 ||
			edgeList[ checkPtr->edge ].end <= dy1 ) {
		    continue ;
		}
		check = 0 ;
		break ;
	    }
	    if( check ) {
		edgeList[++numProbes + edgeCount].start  = dx ;
		edgeList[numProbes + edgeCount].end      = u1x  ;
		edgeList[numProbes + edgeCount].loc      = u1y2 ;
		edgeList[numProbes + edgeCount].length   = u1x - dx ;
		edgeList[numProbes + edgeCount].fixed    = 0 ;
		edgeList[numProbes + edgeCount].cell     = 0 ;
		edgeList[numProbes + edgeCount].UorR     = 1 ;
		tinsert( &hEdgeRoot, u1y2 , numProbes + edgeCount ) ;
#ifdef DEBUG
		fprintf(fpdebug,"hprobe:%d  ", numProbes ) ;
		fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
					dx , u1x , u1y2 , 1 ) ;
#endif
		edgeList[++numProbes + edgeCount].start  = dx ;
		edgeList[numProbes + edgeCount].end      = u1x  ;
		edgeList[numProbes + edgeCount].loc      = dy1 ;
		edgeList[numProbes + edgeCount].length   = u1x - dx ;
		edgeList[numProbes + edgeCount].fixed    = 0 ;
		edgeList[numProbes + edgeCount].cell     = 0 ;
		edgeList[numProbes + edgeCount].UorR     = -1 ;
		tinsert( &hEdgeRoot, dy1 , numProbes + edgeCount ) ;
#ifdef DEBUG
		fprintf(fpdebug,"hprobe:%d  ", numProbes ) ;
		fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
					dx , u1x , dy1 , -1 ) ;
#endif
	    }
	}
    }
    doubleBack( downPtr ) ;
}
return ;
}



void doubleBack( DLINK1PTR dptr )
{

int dx , dy1 , dy2 , ux , uy1 , uy2 , check , edge ;
DLINK1PTR checkPtr , ptr ;

dx  = edgeList[ dptr->edge ].loc   ;
dy2 = edgeList[ dptr->edge ].end   ;
dy1 = edgeList[ dptr->edge ].start ;

ptr = Vptrs[ tprop( Vroot , dx ) ] ;
for( ; ptr != (DLINK1PTR) NULL ; ptr = ptr->next ) {
    edge = ptr->edge ;
    if( edgeList[edge].UorR < 0 ) {
	continue ;
    }
    ux  = edgeList[edge].loc   ;
    uy1 = edgeList[edge].start ;
    uy2 = edgeList[edge].end   ;
    if( ! ( uy2 < dy2 && uy1 > dy1 )  ) {
	continue ;
    }
    if( edgeList[ edgeList[edge].prevEdge ].UorR == -1 &&
		edgeList[ edgeList[edge].nextEdge ].UorR == 1 ) {
	check = 1 ;
	checkPtr = Vptrs[ tprop( Vroot , dx ) ] ;
	for(; checkPtr != (DLINK1PTR) NULL ; checkPtr = checkPtr->next){
	    if( checkPtr == dptr ) {
		continue ;
	    }
	    if( edgeList[ checkPtr->edge ].UorR > 0 ) {
		continue ;
	    }
	    if( edgeList[ checkPtr->edge ].loc > ux ) {
		break ;
	    }
	    if( edgeList[ checkPtr->edge ].start >= uy2 ||
			edgeList[ checkPtr->edge ].end <= uy1 ) {
		continue ;
	    }
	    check = 0 ;
	    break ;
	}
    } else {
	check = 0 ;
    }
    if( check ) {
	edgeList[++numProbes + edgeCount].start  = dx ;
	edgeList[numProbes + edgeCount].end      = ux ;
	edgeList[numProbes + edgeCount].loc      = uy2 ;
	edgeList[numProbes + edgeCount].length   = ux - dx ;
	edgeList[numProbes + edgeCount].fixed    = 0 ;
	edgeList[numProbes + edgeCount].cell     = 0 ;
	edgeList[numProbes + edgeCount].UorR     = 1 ;
	tinsert( &hEdgeRoot, uy2 , numProbes + edgeCount ) ;
#ifdef DEBUG
	fprintf(fpdebug,"hprobe:%d  start:%d  end:%d  loc:%d  UorR:%d\n",
		    numProbes, dx , ux , uy2 , 1 ) ;
#endif
	edgeList[++numProbes + edgeCount].start  = dx ;
	edgeList[numProbes + edgeCount].end      = ux ;
	edgeList[numProbes + edgeCount].loc      = uy1 ;
	edgeList[numProbes + edgeCount].length   = ux - dx ;
	edgeList[numProbes + edgeCount].fixed    = 0 ;
	edgeList[numProbes + edgeCount].cell     = 0 ;
	edgeList[numProbes + edgeCount].UorR     = -1 ;
	tinsert( &hEdgeRoot, uy1 , numProbes + edgeCount ) ;
#ifdef DEBUG
	fprintf(fpdebug,"hprobe:%d  start:%d  end:%d  loc:%d  UorR:%d\n",
		    numProbes, dx , ux , uy1 , -1 ) ;
#endif
    }
}
return ;
}
