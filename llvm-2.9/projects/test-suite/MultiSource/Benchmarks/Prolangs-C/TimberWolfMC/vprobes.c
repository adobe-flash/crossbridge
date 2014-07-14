#include "geo.h"
#define DEBUG

extern int tprop( TNODEPTR r , int value );
extern void tinsert( TNODEPTR *root , int value , int property );

void doubleDown( DLINK1PTR rptr );

void vprobes(void)
{

DLINK1PTR ritePtr , left1ptr , left2ptr , checkPtr , ptr ;
int ry , rx1 , rx2 , redge , dx1 , dx2 , ledge , edge , check ;
int l1x2 , l1x1 , l1y , l2x2 , l2x1 , l2y ;

ritePtr = hFixedList ;
for( ; ritePtr != (DLINK1PTR) NULL ; ritePtr = ritePtr->next ) {
    redge = ritePtr->edge ;
    if( edgeList[redge].UorR < 0 ) {
	continue ;
    }
    ry  = edgeList[redge].loc   ;
    rx1 = edgeList[redge].start ;
    rx2 = edgeList[redge].end   ;
    
    if( edgeList[ edgeList[redge].prevEdge ].UorR == 1 ) {
	left1ptr = Hptrs[ tprop( Hroot , ry ) ] ;
	for(; left1ptr != (DLINK1PTR)NULL; left1ptr = left1ptr->next){
	    ledge = left1ptr->edge ;
	    if( edgeList[ledge].UorR > 0 ) {
		continue ;
	    }
	    l1y  = edgeList[ledge].loc   ;
	    l1x1 = edgeList[ledge].start ;
	    l1x2 = edgeList[ledge].end   ;
	    if( l1x2 <= rx1 || l1x1 > rx1 ) {
		continue ;
	    }
	    break ;
	}
    } else {
	left1ptr = (DLINK1PTR) NULL ;
    }
    if( edgeList[ edgeList[redge].nextEdge ].UorR == -1 ) {
	left2ptr = Hptrs[ tprop( Hroot , ry ) ] ;
	for(; left2ptr != (DLINK1PTR) NULL ; left2ptr = left2ptr->next){
	    ledge = left2ptr->edge ;
	    if( edgeList[ledge].UorR > 0 ) {
		continue ;
	    }
	    l2y  = edgeList[ledge].loc   ;
	    l2x1 = edgeList[ledge].start ;
	    l2x2 = edgeList[ledge].end   ;
	    if( l2x2 < rx2 || l2x1 >= rx2 ) {
		continue ;
	    }
	    break ;
	}
    } else {
	left2ptr = (DLINK1PTR) NULL ;
    }
    if( left1ptr != (DLINK1PTR) NULL && left1ptr == left2ptr ) {
	check = 1 ;
	checkPtr = Hptrs[ tprop( Hroot , ry ) ] ;
	for(; checkPtr != (DLINK1PTR)NULL; checkPtr = checkPtr->next){
	    if( edgeList[ checkPtr->edge ].UorR > 0 ) {
		continue ;
	    }
	    if( edgeList[ checkPtr->edge ].loc >= l2y ) {
		break ;
	    }
	    if( edgeList[ checkPtr->edge ].start >= rx2 ||
			edgeList[ checkPtr->edge ].end <= rx1 ) {
		continue ;
	    }
	    check = 0 ;
	    break ;
	}
	if( check ) {
	    edgeList[++numProbes + edgeCount].start  = ry ;
	    edgeList[numProbes + edgeCount].end      = l2y  ;
	    edgeList[numProbes + edgeCount].loc      = rx1 ;
	    edgeList[numProbes + edgeCount].length   = l2y - ry ;
	    edgeList[numProbes + edgeCount].fixed    = 0 ;
	    edgeList[numProbes + edgeCount].cell     = 0 ;
	    edgeList[numProbes + edgeCount].UorR     = 1 ;
	    tinsert( &vEdgeRoot, rx1 , numProbes + edgeCount ) ;
#ifdef DEBUG
	    fprintf(fpdebug,"vprobe:%d  ", numProbes ) ;
	    fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
					ry , l2y , rx1 , 1 ) ;
#endif
	    edgeList[++numProbes + edgeCount].start  = ry ;
	    edgeList[numProbes + edgeCount].end      = l2y  ;
	    edgeList[numProbes + edgeCount].loc      = rx2 ;
	    edgeList[numProbes + edgeCount].length   = l2y - ry ;
	    edgeList[numProbes + edgeCount].fixed    = 0 ;
	    edgeList[numProbes + edgeCount].cell     = 0 ;
	    edgeList[numProbes + edgeCount].UorR     = -1 ;
	    tinsert( &vEdgeRoot, rx2 , numProbes + edgeCount ) ;
#ifdef DEBUG
	    fprintf(fpdebug,"vprobe:%d  ", numProbes ) ;
	    fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
					ry , l2y , rx2 , -1 ) ;
#endif
	} else {
	    doubleDown( ritePtr ) ;	
	}
	continue ;
    }
    if( left1ptr != (DLINK1PTR) NULL &&
	    edgeList[ edgeList[left1ptr->edge].prevEdge ].UorR == -1 ) {
	ptr = Hptrs[ tprop( Hroot , l1y ) ] ;
	for( ptr = ptr->next; ptr != (DLINK1PTR) NULL; ptr = ptr->next){
	    if( edgeList[ptr->edge].loc > l1y ) {
		break ;
	    }
	}
	if( ptr == (DLINK1PTR) NULL ) {
	    ptr = hFixedEnd ;
	} else {
	    ptr = ptr->prev ;
	}
	for( ; ptr != (DLINK1PTR) NULL; ptr = ptr->prev){
	    edge = ptr->edge ;
	    if( edgeList[edge].UorR < 0 ) {
		continue ;
	    }
	    dx1 = edgeList[edge].start ;
	    dx2 = edgeList[edge].end   ;
	    if( dx2 < l1x2 || dx1 >= l1x2 ) {
		continue ;
	    }
	    break ;
	}
	if( ritePtr == ptr ) {
	    check = 1 ;
	    checkPtr = Hptrs[ tprop( Hroot , ry ) ] ;
	    for(; checkPtr != (DLINK1PTR) NULL ;
				checkPtr = checkPtr->next ) {
		if( edgeList[ checkPtr->edge ].UorR > 0 ) {
		    continue ;
		}
		if( edgeList[ checkPtr->edge ].loc >= l1y ) {
		    break ;
		}
		if( edgeList[ checkPtr->edge ].start >= l1x2 ||
			edgeList[ checkPtr->edge ].end <= rx1 ) {
		    continue ;
		}
		check = 0 ;
		break ;
	    }
	    if( check ) {
		edgeList[++numProbes + edgeCount].start  = ry ;
		edgeList[numProbes + edgeCount].end      = l1y ;
		edgeList[numProbes + edgeCount].loc      = rx1 ;
		edgeList[numProbes + edgeCount].length   = l1y - ry ;
		edgeList[numProbes + edgeCount].fixed    = 0 ;
		edgeList[numProbes + edgeCount].cell     = 0 ;
		edgeList[numProbes + edgeCount].UorR     = 1 ;
		tinsert( &vEdgeRoot, rx1 , numProbes + edgeCount ) ;
#ifdef DEBUG
		fprintf(fpdebug,"vprobe:%d  ", numProbes ) ;
		fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
					ry , l1y , rx1 , 1 ) ;
#endif
		edgeList[++numProbes + edgeCount].start  = ry ;
		edgeList[numProbes + edgeCount].end      = l1y  ;
		edgeList[numProbes + edgeCount].loc      = l1x2 ;
		edgeList[numProbes + edgeCount].length   = l1y - ry ;
		edgeList[numProbes + edgeCount].fixed    = 0 ;
		edgeList[numProbes + edgeCount].cell     = 0 ;
		edgeList[numProbes + edgeCount].UorR     = -1 ;
		tinsert( &vEdgeRoot, l1x2 , numProbes + edgeCount ) ;
#ifdef DEBUG
		fprintf(fpdebug,"vprobe:%d  ", numProbes ) ;
		fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
					ry , l1y , l1x2 , -1 ) ;
#endif
	    }
	}
    }
    if( left2ptr != (DLINK1PTR) NULL &&
	    edgeList[ edgeList[left2ptr->edge].nextEdge ].UorR == 1 ) {
	ptr = Hptrs[ tprop( Hroot , l2y ) ] ;
	for( ptr = ptr->next; ptr != (DLINK1PTR) NULL; ptr = ptr->next){
	    if( edgeList[ptr->edge].loc > l2y ) {
		break ;
	    }
	}
	if( ptr == (DLINK1PTR) NULL ) {
	    ptr = hFixedEnd ;
	} else {
	    ptr = ptr->prev ;
	}
	for( ; ptr != (DLINK1PTR) NULL; ptr = ptr->prev){
	    edge = ptr->edge ;
	    if( edgeList[edge].UorR < 0 ) {
		continue ;
	    }
	    dx1 = edgeList[edge].start ;
	    dx2 = edgeList[edge].end   ;
	    if( dx2 <= l2x1 || dx1 > l2x1 ) {
		continue ;
	    }
	    break ;
	}
	if( ritePtr == ptr ) {
	    check = 1 ;
	    checkPtr = Hptrs[ tprop( Hroot , ry ) ] ;
	    for(; checkPtr != (DLINK1PTR) NULL ;
				    checkPtr = checkPtr->next ) {
		if( edgeList[ checkPtr->edge ].UorR > 0 ) {
		    continue ;
		}
		if( edgeList[ checkPtr->edge ].loc >= l2y ) {
		    break ;
		}
		if( edgeList[ checkPtr->edge ].start >= rx2 ||
			edgeList[ checkPtr->edge ].end <= l2x1 ) {
		    continue ;
		}
		check = 0 ;
		break ;
	    }
	    if( check ) {
		edgeList[++numProbes + edgeCount].start  = ry ;
		edgeList[numProbes + edgeCount].end      = l2y ;
		edgeList[numProbes + edgeCount].loc      = l2x1 ;
		edgeList[numProbes + edgeCount].length   = l2y - ry ;
		edgeList[numProbes + edgeCount].fixed    = 0 ;
		edgeList[numProbes + edgeCount].cell     = 0 ;
		edgeList[numProbes + edgeCount].UorR     = 1 ;
		tinsert( &vEdgeRoot, l2x1 , numProbes + edgeCount ) ;
#ifdef DEBUG
		fprintf(fpdebug,"vprobe:%d  ", numProbes ) ;
		fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
					ry , l2y , l2x1 , 1 ) ;
#endif
		edgeList[++numProbes + edgeCount].start  = ry ;
		edgeList[numProbes + edgeCount].end      = l2y  ;
		edgeList[numProbes + edgeCount].loc      = rx2 ;
		edgeList[numProbes + edgeCount].length   = l2y - ry ;
		edgeList[numProbes + edgeCount].fixed    = 0 ;
		edgeList[numProbes + edgeCount].cell     = 0 ;
		edgeList[numProbes + edgeCount].UorR     = -1 ;
		tinsert( &vEdgeRoot, rx2 , numProbes + edgeCount ) ;
#ifdef DEBUG
		fprintf(fpdebug,"vprobe:%d  ", numProbes ) ;
		fprintf(fpdebug,"start:%d  end:%d  loc:%d  UorR:%d\n",
					ry , l2y , rx2 , -1 ) ;
#endif
	    }
	}
    }
    doubleDown( ritePtr ) ;
}
return ;
}



void doubleDown( DLINK1PTR rptr )
{

int ry , rx1 , rx2 , ly , lx1 , lx2 , check , edge ;
DLINK1PTR checkPtr , ptr ;

ry  = edgeList[ rptr->edge ].loc   ;
rx2 = edgeList[ rptr->edge ].end   ;
rx1 = edgeList[ rptr->edge ].start ;

ptr = Hptrs[ tprop( Hroot , ry ) ] ;
for( ; ptr != (DLINK1PTR) NULL ; ptr = ptr->next ) {
    edge = ptr->edge ;
    if( edgeList[edge].UorR > 0 ) {
	continue ;
    }
    ly  = edgeList[edge].loc   ;
    lx1 = edgeList[edge].start ;
    lx2 = edgeList[edge].end   ;
    if( ! ( lx2 < rx2 && lx1 > rx1 )  ) {
	continue ;
    }
    if( edgeList[ edgeList[edge].prevEdge ].UorR == -1 &&
		edgeList[ edgeList[edge].nextEdge ].UorR == 1 ) {
	check = 1 ;
	checkPtr = Hptrs[ tprop( Hroot , ry ) ] ;
	for(; checkPtr != (DLINK1PTR) NULL ; checkPtr = checkPtr->next){
	    if( checkPtr == rptr ) {
		continue ;
	    }
	    if( edgeList[ checkPtr->edge ].UorR < 0 ) {
		continue ;
	    }
	    if( edgeList[ checkPtr->edge ].loc > ly ) {
		break ;
	    }
	    if( edgeList[ checkPtr->edge ].start >= lx2 ||
			edgeList[ checkPtr->edge ].end <= lx1 ) {
		continue ;
	    }
	    check = 0 ;
	    break ;
	}
    } else {
	check = 0 ;
    }
    if( check ) {
	edgeList[++numProbes + edgeCount].start  = ry ;
	edgeList[numProbes + edgeCount].end      = ly ;
	edgeList[numProbes + edgeCount].loc      = lx1 ;
	edgeList[numProbes + edgeCount].length   = ly - ry ;
	edgeList[numProbes + edgeCount].fixed    = 0 ;
	edgeList[numProbes + edgeCount].cell     = 0 ;
	edgeList[numProbes + edgeCount].UorR     = 1 ;
	tinsert( &vEdgeRoot, lx1 , numProbes + edgeCount ) ;
#ifdef DEBUG
	fprintf(fpdebug,"vprobe:%d  start:%d  end:%d  loc:%d  UorR:%d\n",
		    numProbes, ry , ly , lx1 , 1 ) ;
#endif
	edgeList[++numProbes + edgeCount].start  = ry ;
	edgeList[numProbes + edgeCount].end      = ly ;
	edgeList[numProbes + edgeCount].loc      = lx2 ;
	edgeList[numProbes + edgeCount].length   = ly - ry ;
	edgeList[numProbes + edgeCount].fixed    = 0 ;
	edgeList[numProbes + edgeCount].cell     = 0 ;
	edgeList[numProbes + edgeCount].UorR     = -1 ;
	tinsert( &vEdgeRoot, lx2 , numProbes + edgeCount ) ;
#ifdef DEBUG
	fprintf(fpdebug,"vprobe:%d  start:%d  end:%d  loc:%d  UorR:%d\n",
		    numProbes, ry , ly , lx2 , -1 ) ;
#endif
    }
}
return ;
}
