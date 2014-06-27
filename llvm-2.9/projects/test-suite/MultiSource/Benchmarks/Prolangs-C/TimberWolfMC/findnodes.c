#include "geo.h"
#define DEBUG
#ifdef DEBUG
extern FILE *fpNodes ;
#endif
extern int doPlacement ;

extern int tprop( TNODEPTR r , int value );
extern void tinsert( TNODEPTR *root , int value , int property );
extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property);
int backCheck( int l , int r , int b , int t );
void rectlink(void);
void makeRLEtree(void);
void makeRBEtree(void);

void findnodes(void)
{

DLINK1PTR hptr , vrptr , htptr , vlptr , v1ptr , v2ptr ;
int hedge , hstart , hend , hloc  ;
int htedge, htstart, htend, htloc , ht1 , ht2 ;
int vrend , vrstart , vrloc , vredge ;
int vlend , vlstart , vlloc , vledge ;
int l , r , b , t , last , x ;
int bb , tt , done , check , lt , rt ;

numRects  = 0 ;
rectArray = (RECTPTR) malloc( 100 * sizeof( RECT ) ) ;

last = -100000 ;
for( hptr = Hlist ; hptr != (DLINK1PTR) NULL ; hptr = hptr->next ) {
    hedge = hptr->edge ;
    if( edgeList[hedge].UorR < 0 ) {
	continue ;
    }
    hstart = edgeList[hedge].start ;
    hend   = edgeList[hedge].end   ;
    hloc   = edgeList[hedge].loc   ;

    vlptr = Vptrs[ tprop( Vroot , hstart ) ] ;
    vrloc = -100000 ;
    for( ; vlptr != (DLINK1PTR) NULL; vlptr = vlptr->next ){
	vledge = vlptr->edge ;
	if( edgeList[vledge].UorR > 0 ) {
	    continue ;
	}
	vlloc = edgeList[vledge].loc ;
	if( vlloc > hend ) {
	    break ;
	}
	if( vlloc < vrloc ) {
	    continue ;
	}
	vlstart = edgeList[vledge].start ;
	vlend   = edgeList[vledge].end   ;
	
	if( vlstart > hloc || vlend < hloc ) {
	    continue ;
	}
	if( edgeList[hedge].fixed == 0 ) {
	    if( !( (edgeList[vledge].fixed == 0) ||
		   (edgeList[vledge].fixed == 1 && vlloc == hstart) ) ){
		continue ;
	    }
	} else {
	    if( edgeList[vledge].fixed == 1 && vlend == hloc ) {
		continue ;
	    }
	}

	b  = hloc  ;
	l  = vlloc ;

	vrptr = Vptrs[ tprop( Vroot , hstart ) ] ;
	for( ; vrptr != (DLINK1PTR) NULL; vrptr = vrptr->next ) {
	    vredge = vrptr->edge ;
	    if( edgeList[vredge].UorR < 0 ) {
		continue ;
	    }
	    vrloc = edgeList[vredge].loc ;
	    if( ! (vrloc >= vlloc)  ) {
		continue ;
	    }
	    vrstart = edgeList[vredge].start ;
	    vrend   = edgeList[vredge].end   ;
	
	    if( vrstart > hloc || vrend < hloc ) {
		continue ;
	    }
	    if( edgeList[hedge].fixed == 0 ) {
		if( !( (edgeList[vredge].fixed == 0) ||
		       (edgeList[vredge].fixed == 1 && vrloc == hend) ) ){
		    continue ;
		}
	    } else {
		if( edgeList[vredge].fixed == 1 && vrend == hloc ) {
		    continue ;
		}
	    }

	    r  = vrloc  ;

	    htptr = Hptrs[ tprop( Hroot , hloc ) ] ;
	    done = 0 ;
	    for( ; htptr != (DLINK1PTR) NULL ; htptr = htptr->next ) {
		htedge = htptr->edge ;
		if( edgeList[htedge].UorR > 0 ) {
		    continue ;
		}
		htloc = edgeList[htedge].loc ;
		if( ! (htloc >= hloc) ) {
		    continue ;
		}
		htstart = edgeList[htedge].start ;
		htend   = edgeList[htedge].end   ;
		if( htstart > r || htend < l ) {
		    continue ;
		}
		if( r == l ) { 
		    if( (htstart < l || (htstart == l && 
					 (edgeList[htedge].fixed == 0 ||
					 (edgeList[htedge].UorR < 0 &&
					  edgeList[ edgeList[htedge].
					  nextEdge ].UorR < 0) ) )) && 
				      (htend > r || (htend == r &&
					 (edgeList[htedge].fixed == 0 ||
					 (edgeList[htedge].UorR < 0 &&
					  edgeList[ edgeList[htedge].
					  prevEdge ].UorR > 0 ) ) )) ) {
			ht1 = ht2 = htedge ;
			done = 4 ;
		    }
		} else {
		    if( done == 0 ) {
			if( (htstart < l || (htstart == l && 
					 (edgeList[htedge].fixed == 0 ||
					 (edgeList[htedge].UorR < 0 &&
					  edgeList[ edgeList[htedge].
					  nextEdge ].UorR < 0) ) )) && 
				  (htend > l || (htend == l &&
				     edgeList[htedge].fixed == 0 )) ) {

			    if( (htstart < r || (htstart == r && 
				      edgeList[htedge].fixed == 0 )) && 
				  (htend > r || (htend == r &&
					 (edgeList[htedge].fixed == 0 ||
					 (edgeList[htedge].UorR < 0 &&
					  edgeList[ edgeList[htedge].
					  prevEdge ].UorR > 0 ) ) )) ) {
				ht1 = ht2 = htedge ;
				done = 4 ;
			    }
			}
		    }
		    if( done == 1 || done == 0 ) {
			if( (htstart < r || (htstart == r && 
				      edgeList[htedge].fixed == 0 )) && 
				  (htend > r || (htend == r &&
					 (edgeList[htedge].fixed == 0 ||
					 (edgeList[htedge].UorR < 0 &&
					  edgeList[ edgeList[htedge].
					  prevEdge ].UorR > 0 ) ) )) ) {
			    ht2 = htedge ;
			    if( done == 1 ) {
				done = 3 ;
			    } else {
				done = 2 ;
			    }
			}
		    }
		    if( done == 2 || done == 0 ) {
			if( (htstart < l || (htstart == l && 
					 (edgeList[htedge].fixed == 0 ||
					 (edgeList[htedge].UorR < 0 &&
					  edgeList[ edgeList[htedge].
					  nextEdge ].UorR < 0) ) )) && 
				  (htend > l || (htend == l &&
				     edgeList[htedge].fixed == 0 )) ) {
			    ht1 = htedge ;
			    if( done == 2 ) {
				done = 3 ;
			    } else {
				done = 1 ;
			    }
			}
		    }
		}
		if( done < 3 ) {
		    continue ;
		} else {
		    break ;
		}
	    }
	    if( done < 3 ) {
		break ;
	    }
	    t = htloc  ;
	    if( edgeList[ht1].loc < edgeList[ht2].loc ) {
		lt = edgeList[ht2].start ;
		rt = edgeList[ht2].end   ;
	    } else if( edgeList[ht1].loc > edgeList[ht2].loc ) {
		lt = edgeList[ht1].start ;
		rt = edgeList[ht1].end   ;
	    } else {
		lt = edgeList[ht1].start ;
		rt = edgeList[ht2].end   ;
	    }
	    if( vlend <= t && lt < l ) {
		check = edgeList[ht1].fixed ;
		v1ptr = Vptrs[ tprop( Vroot , l ) ] ;
		x = edgeList[ v1ptr->edge ].loc ;
		v1ptr = v1ptr->next ;
		for( ; v1ptr != (DLINK1PTR) NULL; 
				v1ptr = v1ptr->next ){
		    if( edgeList[ v1ptr->edge ].loc > x ) {
			break ;
		    }
		}
		v1ptr = v1ptr->prev ;
		for( ; v1ptr != (DLINK1PTR) NULL; 
				v1ptr = v1ptr->prev ){
		    if( edgeList[ v1ptr->edge ].UorR > 0 ) {
			continue ;
		    }
		    tt = edgeList[ v1ptr->edge ].end ;
		    bb = edgeList[ v1ptr->edge ].start ;

		    if( bb > t || tt < t ) {
			continue ;
		    }
		    if( check ) {
			if( bb == t && tt > bb ) {
			    continue ;
			}
		    } else {
			if( (edgeList[ v1ptr->edge ].fixed == 1) &&
					    (bb == t || tt == t) ) {
			    continue ;
			}
		    }
		    l = edgeList[ v1ptr->edge ].loc ;
		    break ;
		}
	    }
	    if( vrend <= t && rt > r ) {
		check = edgeList[ht2].fixed ;
		v2ptr = Vptrs[ tprop( Vroot , r ) ] ;
		for( ; v2ptr != (DLINK1PTR) NULL; 
				v2ptr = v2ptr->next ){
		    if( edgeList[ v2ptr->edge ].UorR < 0 ) {
			continue ;
		    }
		    tt = edgeList[ v2ptr->edge ].end ;
		    bb = edgeList[ v2ptr->edge ].start ;

		    if( bb > t || tt < t ) {
			continue ;
		    }
		    if( check ) {
			if( bb == t && tt > bb ) {
			    continue ;
			}
		    } else {
			if( (edgeList[ v2ptr->edge ].fixed == 1) &&
					    (bb == t || tt == t) ) {
			    continue ;
			}
		    }
		    r = edgeList[ v2ptr->edge ].loc ;
		    break ;
		}
	    }
	    if( backCheck( l , r , b , t ) ) {
		break ;
	    }
	    if( ++numRects % 100 == 0 ) {
		rectArray = (RECTPTR) realloc( rectArray ,
			    (numRects + 100) * sizeof(RECT) ) ;
	    }
	    rectArray[numRects].l = l   ;
	    rectArray[numRects].r = r   ;
	    rectArray[numRects].b = b   ;
	    rectArray[numRects].t = t   ;
	    rectArray[numRects].ur = 0  ;
	    rectArray[numRects].ul = 0  ;
	    rectArray[numRects].lr = 0  ;
	    rectArray[numRects].ll = 0  ;
	    rectArray[numRects].xc = (l + r) / 2 ;
	    rectArray[numRects].yc = (b + t) / 2 ;
	    rectArray[numRects].xreset = 0 ;
	    rectArray[numRects].yreset = 0 ;
	    tinsert( &hRectRoot , l , numRects ) ;
	    tinsert( &vRectRoot , b , numRects ) ;
#ifdef DEBUG
	    fprintf(fpdebug,"rect Node: %d  at: %d %d ",
			numRects , rectArray[numRects].xc ,
			rectArray[numRects].yc ) ;
	    fprintf(fpdebug,"  width:%d  height:%d\n",
				     r - l , t - b ) ;
	    if( doPlacement ) {
		fprintf(fpNodes,"L NC;\n94 X%d %d %d;\n",
			    numRects , (r + l)/2 , (t + b)/2 );
	    }
#endif
	    break ;
	}
    }
}
rectlink() ;
makeRLEtree() ;
makeRBEtree() ;

#ifdef DEBUG
if( doPlacement ) {
    fprintf( fpNodes , "E\n" );
    fclose( fpNodes ) ;
}
#endif

return ;
}



int backCheck( int l , int r , int b , int t )
{

int rec , lChk , rChk , bChk , tChk ;

for( rec = numRects ; rec >= 1 ; rec-- ) {
    lChk = rectArray[rec].l ;
    rChk = rectArray[rec].r ;
    if( l > rChk || r < lChk ) {
	continue ;
    }
    break ;
}
if( rec == 0 ) {
    return(0) ;
} else {
    bChk = rectArray[rec].b ;
    tChk = rectArray[rec].t ;
    if( b > tChk || t < bChk ) {
	return(0) ;
    } else {
	return(1) ;
    }
}
}


void rectlink(void)
{

TNODEPTR junkptr ;
DLINK2PTR ptr , pptr ;
int center , index ;

HRlist = (DLINK2PTR) NULL ;
VRlist = (DLINK2PTR) NULL ;

tpop( &hRectRoot , &junkptr , &center , &index ) ;
if( junkptr == (TNODEPTR) NULL ) {
    return ;
}
HRlist = (DLINK2PTR) malloc( sizeof( DLINK2 ) ) ;
HRlist->index  = index  ;
HRlist->next = (DLINK2PTR) NULL ;
HRlist->prev = (DLINK2PTR) NULL ;

tpop( &vRectRoot , &junkptr , &center , &index ) ;
VRlist = (DLINK2PTR) malloc( sizeof( DLINK2 ) ) ;
VRlist->index  = index  ;
VRlist->next = (DLINK2PTR) NULL ;
VRlist->prev = (DLINK2PTR) NULL ;

pptr = HRlist ;
for( ; ; ) {
    tpop( &hRectRoot , &junkptr , &center , &index ) ;
    if( junkptr == (TNODEPTR) NULL ) {
	break ;
    }
    ptr = (DLINK2PTR) malloc( sizeof( DLINK2 ) ) ;
    ptr->index  = index ;
    ptr->prev = pptr ;
    ptr->next = (DLINK2PTR) NULL ;
    pptr->next = ptr ;
    pptr = ptr ;
}

pptr = VRlist ;
for( ; ; ) {
    tpop( &vRectRoot , &junkptr , &center , &index ) ;
    if( junkptr == (TNODEPTR) NULL ) {
	break ;
    }
    ptr = (DLINK2PTR) malloc( sizeof( DLINK2 ) ) ;
    ptr->index  = index ;
    ptr->prev = pptr ;
    ptr->next = (DLINK2PTR) NULL ;
    pptr->next = ptr ;
    pptr = ptr ;
}
return ;
}



void makeRLEtree(void)
{

DLINK2PTR leptr ;
int last , index , count ;

LEroot = (TNODEPTR) NULL ;
LEptrs = (DLINK2PTR *) malloc( 100 * sizeof(DLINK2PTR) ) ;
count = 0 ;

last = -1000000 ;
for( leptr = HRlist ; leptr != (DLINK2PTR) NULL ; leptr = leptr->next ){
    index = leptr->index ;
    if( rectArray[index].l > last ) {
	last = rectArray[index].l ;
	if( ++count % 100 == 0 ) {
	    LEptrs = (DLINK2PTR *) realloc( LEptrs ,
				(count + 100) * sizeof(DLINK2PTR) ) ;
	}
	LEptrs[count] = leptr ;
	tinsert( &LEroot , last , count ) ;
    }
}
return ;
}



void makeRBEtree(void)
{

DLINK2PTR beptr ;
int last , index , count ;

BEroot = (TNODEPTR) NULL ;
BEptrs = (DLINK2PTR *) malloc( 100 * sizeof(DLINK2PTR) ) ;
count = 0 ;

last = -1000000 ;
for( beptr = VRlist ; beptr != (DLINK2PTR) NULL ; beptr = beptr->next ){
    index = beptr->index ;
    if( rectArray[index].b > last ) {
	last = rectArray[index].b ;
	if( ++count % 100 == 0 ) {
	    BEptrs = (DLINK2PTR *) realloc( BEptrs ,
				(count + 100) * sizeof(DLINK2PTR) ) ;
	}
	BEptrs[count] = beptr ;
	tinsert( &BEroot , last , count ) ;
    }
}
return ;
}
