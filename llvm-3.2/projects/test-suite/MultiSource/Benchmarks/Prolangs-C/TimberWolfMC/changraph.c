#include "geo.h"
#define DEBUG

extern int tprop( TNODEPTR r , int value );
extern void tinsert( TNODEPTR *root , int value , int property );

void exploreUp(void);
void exploreRite(void);
int constructVedge( int index1 , int index2 );
int constructHedge( int index1 , int index2 );

void changraph(void)
{
#ifdef notdef
char filename[1024] ;
FILE *fp ;
#endif
int i , index1 , index2 , hiend , loend , length ;

vChanBeginRoot = (TNODEPTR) NULL ;
vChanEndRoot   = (TNODEPTR) NULL ;
hChanBeginRoot = (TNODEPTR) NULL ;
hChanEndRoot   = (TNODEPTR) NULL ;
eNum = 0 ;
eArray = (EBOXPTR) malloc( 100 * sizeof(EBOX) ) ;

/* XXX Ahh, Sechen code, lint torture testing */
/* exploreUp( HRlist , 0 , -1000000 ) ; */
exploreUp();

edgeTransition = eNum ;

/* exploreRite( VRlist , 0 , -1000000 ) ; */
exploreRite();

eIndexArray = (int **) malloc( (1 + numRects) * sizeof( int * ) ) ;
for( i = 1 ; i <= numRects ; i++ ) {
    eIndexArray[i] = (int *) malloc( (1 + numRects) * sizeof( int ) ) ;
}

#ifdef notdef
sprintf( filename, "%s.gph", cktName ) ;
fp = fopen( filename , "w" ) ;
#endif

for( i = 1 ; i <= eNum ; i++ ) {
    index1 = eArray[i].index1 ;
    index2 = eArray[i].index2 ;
    eIndexArray[index1][index2] = i ;
    eIndexArray[index2][index1] = i ;
    if( i <= edgeTransition ) {
	hiend = rectArray[index2].b ;
	loend = rectArray[index1].t ;
	length = rectArray[index2].yc - rectArray[index1].yc  ;
	tinsert( &vChanBeginRoot , eArray[i].lbside , i ) ;
	tinsert( &vChanEndRoot , eArray[i].rtside , i ) ;
    } else {
	hiend = rectArray[index2].l ;
	loend = rectArray[index1].r ;
	length = rectArray[index2].xc - rectArray[index1].xc  ;
	tinsert( &hChanBeginRoot , eArray[i].lbside , i ) ;
	tinsert( &hChanEndRoot , eArray[i].rtside , i ) ;
    }
    eArray[i].length = length ;
    eArray[i].hiend = hiend ;
    eArray[i].loend = loend ;
#ifdef notdef
    fprintf( fp, "edge %5d %5d   length %8d  capacity %8d\n",
		    index1 , index2 , length , eArray[i].width ) ;
#endif
}
#ifdef notdef
fclose(fp);
#endif

#ifdef DEBUG
fprintf(fpdebug,"CHANNEL-GRAPH NODES:\n");
for( i = 1 ; i <= numRects ; i++ ) {
    fprintf(fpdebug,"rect Node: %d  at: %d %d  width:%d  height:%d\n",
		i , rectArray[i].xc , rectArray[i].yc ,
		rectArray[i].r - rectArray[i].l ,
				rectArray[i].t - rectArray[i].b ) ;
}
#endif

return ;
}



void exploreUp(void)
{

DLINK2PTR beptr ;
int beg , end , left , rite , finishLine , b , l , r , rec , index ;

for( rec = 1 ; rec <= numRects ; rec++ ) {
    beg  = rectArray[rec].b ;
    end  = rectArray[rec].t ;
    rite = rectArray[rec].r ;
    left = rectArray[rec].l ;
    finishLine = 1000000 ;
    beptr = BEptrs[ tprop( BEroot , beg ) ] ;
    for( ; beptr != (DLINK2PTR) NULL; beptr = beptr->next ){
	index = beptr->index ;
	b = rectArray[index].b ;
	if( b > finishLine ) {
	    break ;
	}
	l = rectArray[index].l ;
	r = rectArray[index].r ;
	if( b <= end || l > rite || r < left ) {
	    continue ;
	}
	if( constructVedge( rec , index ) ) {
	    break ;
	}
	finishLine = rectArray[index].t ;
    }
}
return ;
}



void exploreRite(void)
{

DLINK2PTR leptr ;
int beg , end , top , bot , finishLine , l , b , t , rec , index ;

for( rec = 1 ; rec <= numRects ; rec++ ) {
    beg = rectArray[rec].l ;
    end = rectArray[rec].r ;
    top = rectArray[rec].t ;
    bot = rectArray[rec].b ;
    finishLine = 1000000 ;
    leptr = LEptrs[ tprop( LEroot , beg ) ] ;
    for( ; leptr != (DLINK2PTR) NULL; leptr = leptr->next ){
	index = leptr->index ;
	l = rectArray[index].l ;
	if( l > finishLine ) {
	    break ;
	}
	b = rectArray[index].b ;
	t = rectArray[index].t ;
	if( l <= end || b > top || t < bot ) {
	    continue ;
	}
	if( constructHedge( rec , index ) ) {
	    break ;
	}
	finishLine = rectArray[index].r ;
    }
}
return ;
}



int constructVedge( int index1 , int index2 )
{

DLINK1PTR v1ptr , v2ptr , vptr ;
int l1 , l2 , r1 , r2 , l , r , b , t , edge1 , edge2 , el , er ;
int eb , et , eb1 , et1 , eb2 , et2 ;
int edge , x ;

l1 = rectArray[index1].l ;
r1 = rectArray[index1].r ;
l2 = rectArray[index2].l ;
r2 = rectArray[index2].r ;
l = ( l1 >= l2 ) ? l1 : l2 ;
r = ( r1 <= r2 ) ? r1 : r2 ;
b = rectArray[index1].t ; 
t = rectArray[index2].b ; 

el = -1000000 ;
v1ptr = VDptrs[ tprop( VDroot , l ) ] ;
for( ; v1ptr != (DLINK1PTR) NULL; v1ptr = v1ptr->next ) {
    edge1 = v1ptr->edge ;
    if( edgeList[edge1].loc > r ) {
	break ;
    }
    if( edgeList[edge1].UorR > 0 ) {
	continue ;
    }
    if( edgeList[edge1].start > b || edgeList[edge1].end < t ) {
	continue ;
    }
    el = edgeList[edge1].loc ;
    vptr = v1ptr ;
}
if( el == -1000000 ) {
    return(0) ;
}
v2ptr = Vptrs[ tprop( Vroot , el ) ] ;
for( ; v2ptr != (DLINK1PTR) NULL; v2ptr = v2ptr->next ) {
    edge2 = v2ptr->edge ;
    if( edgeList[edge2].UorR < 0 ) {
	continue ;
    }
    if( edgeList[edge2].start > b || edgeList[edge2].end < t ) {
	continue ;
    }
    er = edgeList[edge2].loc ;
    break ;
}
if( er > r ) {
    return(0) ;
}
v1ptr = vptr ;
edge1 = v1ptr->edge ;

et1 = edgeList[edge1].end ;
eb1 = edgeList[edge1].start ;
et2 = edgeList[edge2].end ;
eb2 = edgeList[edge2].start ;
if( edgeList[edge1].fixed == 1 && edgeList[edge2].fixed == 1 ) {
    if( edgeList[edge1].cell > 0 && edgeList[edge2].cell > 0 ) {
	et = (et1 >= et2) ? et1 : et2 ;
	eb = (eb1 <= eb2) ? eb1 : eb2 ;
    } else if( edgeList[edge1].cell > 0 ) {
	et = et1 ;
	eb = eb1 ;
    } else if( edgeList[edge2].cell > 0 ) {
	et = et2 ;
	eb = eb2 ;
    } else {
	et = 1000000 ;
	eb = -1000000 ;
    }
} else if( edgeList[edge1].fixed == 1 ) {
    if( edgeList[edge1].cell > 0 ) {
	et = et1 ;
	eb = eb1 ;
    } else {
	et = 1000000 ;
	eb = -1000000 ;
    }
} else if( edgeList[edge2].fixed == 1 ) {
    if( edgeList[edge2].cell > 0 ) {
	et = et2 ;
	eb = eb2 ;
    } else {
	et = 1000000 ;
	eb = -1000000 ;
    }
} else {
    et = 1000000 ;
    eb = -1000000 ;
}
if( ! ( et > rectArray[index2].t ) ){
    if( rectArray[index2].yc < et ) {
	rectArray[index2].yc = et ;
	rectArray[index2].yreset = 2 ;
    }
}
if( ! ( eb < rectArray[index1].b ) ){
    if( rectArray[index1].yc > eb ) {
	rectArray[index1].yc = eb ;
	rectArray[index1].yreset = 1 ;
    }
}
if( rectArray[index1].xc < el || rectArray[index1].xc > er ) {
    rectArray[index1].xc = (el + er) / 2 ;
}
if( rectArray[index2].xc < el || rectArray[index2].xc > er ) {
    rectArray[index2].xc = (el + er) / 2 ;
}

if( edgeList[edge1].fixed == 0 ) {
    vptr = Vptrs[ tprop( Vroot , el ) ] ;
    x = edgeList[vptr->edge].loc ;
    vptr = vptr->next ;
    for( ; vptr != (DLINK1PTR) NULL; vptr = vptr->next ) {
	if( edgeList[vptr->edge].loc > x ) {
	    break ;
	}
    }
    for( vptr = vptr->prev; vptr != (DLINK1PTR) NULL; 
					    vptr = vptr->prev ) {
	if( vptr == v2ptr ) {
	    continue ;
	}
	edge = vptr->edge ;
	if( edgeList[edge].UorR < 0 ) {
	    continue ;
	}
	if( edgeList[edge].start > b || edgeList[edge].end < t ) {
	    continue ;
	}
	break ;
    }
    el -= (el - edgeList[edge].loc) - (el - edgeList[edge].loc) / 2 ;
}
if( edgeList[edge2].fixed == 0 ) {
    vptr = Vptrs[ tprop( Vroot , er ) ] ;
    for(; vptr != (DLINK1PTR) NULL; vptr = vptr->next ) {
	if( vptr == v1ptr ) {
	    continue ;
	}
	edge = vptr->edge ;
	if( edgeList[edge].UorR > 0 ) {
	    continue ;
	}
	if( edgeList[edge].start > b || edgeList[edge].end < t ) {
	    continue ;
	}
	break ;
    }
    er += (edgeList[edge].loc - er) / 2 ;
}

if( ++eNum % 100 == 0 ) {
    eArray = (EBOXPTR) realloc( eArray, (eNum + 100) * sizeof(EBOX));
}
eArray[eNum].index1 = index1 ;
eArray[eNum].index2 = index2 ;
eArray[eNum].width  = er - el ;
eArray[eNum].lbside = el ;
eArray[eNum].rtside = er ;
eArray[eNum].edge1 = edge1 ;
eArray[eNum].edge2 = edge2 ;

return(1) ;
}




int constructHedge( int index1 , int index2 )
{

DLINK1PTR h1ptr , h2ptr , hptr ;
int b1 , b2 , t1 , t2 , b , l , r , t , edge1 , edge2 , eb , et ;
int el , er , el1 , er1 , el2 , er2 ;
int edge , x ;

b1 = rectArray[index1].b ;
t1 = rectArray[index1].t ;
b2 = rectArray[index2].b ;
t2 = rectArray[index2].t ;
b = ( b1 >= b2 ) ? b1 : b2 ;
t = ( t1 <= t2 ) ? t1 : t2 ;
l = rectArray[index1].r ; 
r = rectArray[index2].l ; 

eb = -1000000 ;
h1ptr = HRptrs[ tprop( HRroot , b ) ] ;
for( ; h1ptr != (DLINK1PTR) NULL; h1ptr = h1ptr->next ) {
    edge1 = h1ptr->edge ;
    if( edgeList[edge1].loc > t ) {
	break ;
    }
    if( edgeList[edge1].UorR < 0 ) {
	continue ;
    }
    if( edgeList[edge1].start > l || edgeList[edge1].end < r ) {
	continue ;
    }
    eb = edgeList[edge1].loc ;
    hptr = h1ptr ;
}
if( eb == -1000000 ) {
    return(0) ;
}
h2ptr = Hptrs[ tprop( Hroot , eb ) ] ;
for( ; h2ptr != (DLINK1PTR) NULL; h2ptr = h2ptr->next ) {
    edge2 = h2ptr->edge ;
    if( edgeList[edge2].UorR > 0 ) {
	continue ;
    }
    if( edgeList[edge2].start > l || edgeList[edge2].end < r ) {
	continue ;
    }
    et = edgeList[edge2].loc ;
    break ;
}
if( et > t ) {
    return(0) ;
}
h1ptr = hptr ;
edge1 = h1ptr->edge ;

er1 = edgeList[edge1].end ;
el1 = edgeList[edge1].start ;
er2 = edgeList[edge2].end ;
el2 = edgeList[edge2].start ;
if( edgeList[edge1].fixed == 1 && edgeList[edge2].fixed == 1 ) {
    if( edgeList[edge1].cell > 0 && edgeList[edge2].cell > 0 ) {
	er = (er1 >= er2) ? er1 : er2 ;
	el = (el1 <= el2) ? el1 : el2 ;
    } else if( edgeList[edge1].cell > 0 ) {
	er = er1 ;
	el = el1 ;
    } else if( edgeList[edge2].cell > 0 ) {
	er = er2 ;
	el = el2 ;
    } else {
	er = 1000000 ;
	el = -1000000 ;
    }
} else if( edgeList[edge1].fixed == 1 ) {
    if( edgeList[edge1].cell > 0 ) {
	er = er1 ;
	el = el1 ;
    } else {
	er = 1000000 ;
	el = -1000000 ;
    }
} else if( edgeList[edge2].fixed == 1 ) {
    if( edgeList[edge2].cell > 0 ) {
	er = er2 ;
	el = el2 ;
    } else {
	er = 1000000 ;
	el = -1000000 ;
    }
} else {
    er = 1000000 ;
    el = -1000000 ;
}
if( ! ( er > rectArray[index2].r ) ){
    if( rectArray[index2].xc < er ) {
	rectArray[index2].xc = er ;
	rectArray[index2].xreset = 2 ;
    }
}
if( ! ( el < rectArray[index1].l ) ){
    if( rectArray[index1].xc > el ) {
	rectArray[index1].xc = el ;
	rectArray[index1].xreset = 1 ;
    }
}
if( rectArray[index1].yc < eb || rectArray[index1].yc > et ) {
    rectArray[index1].yc = (eb + et) / 2 ;
}
if( rectArray[index2].yc < eb || rectArray[index2].yc > et ) {
    rectArray[index2].yc = (eb + et) / 2 ;
}

if( edgeList[edge1].fixed == 0 ) {
    hptr = Hptrs[ tprop( Hroot , eb ) ] ;
    x = edgeList[hptr->edge].loc ;
    hptr = hptr->next ;
    for( ; hptr != (DLINK1PTR) NULL; hptr = hptr->next ) {
	if( edgeList[hptr->edge].loc > x ) {
	    break ;
	}
    }
    for( hptr = hptr->prev; hptr != (DLINK1PTR) NULL; 
					    hptr = hptr->prev ) {
	if( hptr == h2ptr ) {
	    continue ;
	}
	edge = hptr->edge ;
	if( edgeList[edge].UorR > 0 ) {
	    continue ;
	}
	if( edgeList[edge].start > l || edgeList[edge].end < r ) {
	    continue ;
	}
	break ;
    }
    eb -= (eb - edgeList[edge].loc) - (eb - edgeList[edge].loc) / 2 ;
}
if( edgeList[edge2].fixed == 0 ) {
    hptr = Hptrs[ tprop( Hroot , et ) ] ;
    for(; hptr != (DLINK1PTR) NULL; hptr = hptr->next ) {
	if( hptr == h1ptr ) {
	    continue ;
	}
	edge = hptr->edge ;
	if( edgeList[edge].UorR < 0 ) {
	    continue ;
	}
	if( edgeList[edge].start > l || edgeList[edge].end < r ) {
	    continue ;
	}
	break ;
    }
    et += (edgeList[edge].loc - et) / 2 ;
}

if( ++eNum % 100 == 0 ) {
    eArray = (EBOXPTR) realloc( eArray, (eNum + 100) * sizeof(EBOX));
}
eArray[eNum].index1 = index1 ;
eArray[eNum].index2 = index2 ;
eArray[eNum].width  = et - eb ;
eArray[eNum].lbside = eb ;
eArray[eNum].rtside = et ;
eArray[eNum].edge1 = edge1 ;
eArray[eNum].edge2 = edge2 ;

return(1) ;
}
