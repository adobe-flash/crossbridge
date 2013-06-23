#include "geo.h"
#include "route.h"
#define DEBUG
extern FILE *fpo ;
extern int defaultTracks ;

extern void tinsert( TNODEPTR *root , int value , int property );

void density(void)
{

int net , m , eIndex , start , end , channelDensity , i ;
int qbot , qtop , c , count , max , width ;
int cell1 , cell2 , edge1 , edge2 , extraSpace1 , extraSpace2 ;
TNODEPTR node , *q ;
CHANBOXPTR cptr ;
PLISTPTR plptr ;
WCPTR ptr ;
CLBOXPTR clptr ;

for( eIndex = 1 ; eIndex <= eNum ; eIndex++ ) {
    eArray[eIndex].root = (TNODEPTR) NULL ;
}

for( net = 1 ; net <= largestNet ; net++ ) {
    m = netRoutes[net].currentRoute ;
    cptr = netRoutes[net].alternate[m]->chanList ;
    for( ; cptr != (CHANBOXPTR) NULL ; cptr = cptr->next ) {
	eIndex = eIndexArray[ cptr->from ][ cptr->to ] ;
	if( cptr->from == eArray[eIndex].index1 ) {
	    start = cptr->foffset ;
	    end   = eArray[eIndex].length - cptr->toffset ;
	} else {
	    start = cptr->toffset ;
	    end   = eArray[eIndex].length - cptr->foffset ;
	}
	if( end > start ) {
	    tinsert( &(eArray[eIndex].root) , start ,  1 ) ;
	    tinsert( &(eArray[eIndex].root) , end + 1 , -1 ) ;
	}
    }
}

#ifdef DEBUG
fprintf(fpo,"CHANNEL DENSITIES:\n");    
#endif
q = (TNODEPTR *) malloc( 10001 * sizeof(TNODEPTR) ) ;
for( eIndex = 1 ; eIndex <= eNum ; eIndex++ ) {
    if( eArray[eIndex].root == (TNODEPTR) NULL ) {
	eArray[eIndex].density = 0 ;
	continue ;
    }
    channelDensity = 0 ;
    max  = 0 ;
    qtop = 0 ;
    qbot = 0 ;
    q[0] = eArray[eIndex].root ;
    while( qtop >= qbot ) {
	node = q[ qbot++ ] ;
	if( node->nsons > 0 ) {
	    q[ ++qtop ] = node->son1 ;
	    q[ ++qtop ] = node->son2 ;
	    if( node->son3 != (TNODEPTR) NULL ) {
		q[ ++qtop ] = node->son3 ;
	    }
	} else {
	    plptr = node->plist ;
	    for( ; plptr != (PLISTPTR) NULL ; plptr = plptr->pnext ){
		channelDensity += node->plist->property ;
	    }
	    if( channelDensity > max ) {
		max = channelDensity ;
	    }
	}
    }
    eArray[eIndex].density = max ;

#ifdef DEBUG
    fprintf(fpo,"channel:%d  node1:%d  node2:%d  density:%d\n",
	eIndex, eArray[eIndex].index1, eArray[eIndex].index2, max );
#endif

}
free(q);

for( i = 1 ; i <= numXnodes ; i++ ) {
    for( ptr = xNodeArray[i] ; ptr != (WCPTR) NULL ; ptr = ptr->next ){
	if( ptr->fixedWidth == 1 ) {
	    continue ;
	}
	ptr->length = 0 ;
	count = ptr->channels[0] ;
	for( c = 1 ; c <= count ; c++ ) {
	    ptr->length += eArray[ ptr->channels[c] ].density ;
	}
	if( ptr->length > 0 ) {
	    ptr->length += defaultTracks + 2 ;
	    ptr->length *= pitch ;
	} else if( defaultTracks > 0 ) {
	    ptr->length = defaultTracks + 1 ;
	    ptr->length *= pitch ;
	}
    }
}

for( i = 1 ; i <= numYnodes ; i++ ) {
    for( ptr = yNodeArray[i] ; ptr != (WCPTR) NULL ; ptr = ptr->next ){
	if( ptr->fixedWidth == 1 ) {
	    continue ;
	}
	ptr->length = 0 ;
	count = ptr->channels[0] ;
	for( c = 1 ; c <= count ; c++ ) {
	    ptr->length += eArray[ ptr->channels[c] ].density ;
	}
	if( ptr->length > 0 ) {
	    ptr->length += defaultTracks + 2 ;
	    ptr->length *= pitch ;
	} else if( defaultTracks > 0 ) {
	    ptr->length = defaultTracks + 1 ;
	    ptr->length *= pitch ;
	}
    }
}


for( i = 1 ; i <= numXnodes ; i++ ) {
    for( ptr = xNodeArray[i] ; ptr != (WCPTR) NULL ; ptr = ptr->next ){
	if( ptr->fixedWidth == 1 ) {
	    continue ;
	}
	width = 0 ;
	edge1 = eArray[ ptr->channels[1] ].edge1 ;
	cell1 = edgeList[edge1].cell ;
	count = ptr->channels[0] ;
	edge2 = eArray[ ptr->channels[count] ].edge2 ;
	cell2 = edgeList[edge2].cell ;
	for( c = 1 ; c <= count ; c++ ) {
	    width += eArray[ ptr->channels[c] ].width ;
	}
	extraSpace1 = ptr->length / 2 ;
	extraSpace2 = ptr->length - (ptr->length / 2) ;
	if( cell1 > 0 ) {
	    clptr = cellList[cell1] ;
	    for( ; ; clptr = clptr->next ) {
		if( clptr->edge == edge1 ) {
		    break ;
		}
	    }
	    if( width - ptr->length < clptr->compact ) {
		clptr->compact = width - ptr->length ;
		clptr->extraSpace = extraSpace1 ;
	    }
	    clptr->HorV  = 0 ;
	    clptr->loc   = edgeList[edge1].loc ;
	    clptr->start = edgeList[edge1].start ;
	    clptr->end   = edgeList[edge1].end ;
	}
	if( cell2 > 0 ) {
	    clptr = cellList[cell2] ;
	    for( ; ; clptr = clptr->next ) {
		if( clptr->edge == edge2 ) {
		    break ;
		}
	    }
	    if( width - ptr->length < clptr->compact ) {
		clptr->compact = width - ptr->length ;
		clptr->extraSpace = extraSpace2 ;
	    }
	    clptr->HorV  = 0 ;
	    clptr->loc   = edgeList[edge2].loc ;
	    clptr->start = edgeList[edge2].start ;
	    clptr->end   = edgeList[edge2].end ;
	}
    }
}
for( i = 1 ; i <= numYnodes ; i++ ) {
    for( ptr = yNodeArray[i] ; ptr != (WCPTR) NULL ; ptr = ptr->next ){
	if( ptr->fixedWidth == 1 ) {
	    continue ;
	}
	width = 0 ;
	edge1 = eArray[ ptr->channels[1] ].edge1 ;
	cell1 = edgeList[edge1].cell ;
	count = ptr->channels[0] ;
	edge2 = eArray[ ptr->channels[count] ].edge2 ;
	cell2 = edgeList[edge2].cell ;
	for( c = 1 ; c <= count ; c++ ) {
	    width += eArray[ ptr->channels[c] ].width ;
	}
	extraSpace1 = ptr->length / 2 ;
	extraSpace2 = ptr->length - (ptr->length / 2) ;
	if( cell1 > 0 ) {
	    clptr = cellList[cell1] ;
	    for( ; ; clptr = clptr->next ) {
		if( clptr->edge == edge1 ) {
		    break ;
		}
	    }
	    if( width - ptr->length < clptr->compact ) {
		clptr->compact = width - ptr->length ;
		clptr->extraSpace = extraSpace1 ;
	    }
	    clptr->HorV  = 1 ;
	    clptr->loc   = edgeList[edge1].loc ;
	    clptr->start = edgeList[edge1].start ;
	    clptr->end   = edgeList[edge1].end ;
	}
	if( cell2 > 0 ) {
	    clptr = cellList[cell2] ;
	    for( ; ; clptr = clptr->next ) {
		if( clptr->edge == edge2 ) {
		    break ;
		}
	    }
	    if( width - ptr->length < clptr->compact ) {
		clptr->compact = width - ptr->length ;
		clptr->extraSpace = extraSpace2 ;
	    }
	    clptr->HorV  = 1 ;
	    clptr->loc   = edgeList[edge2].loc ;
	    clptr->start = edgeList[edge2].start ;
	    clptr->end   = edgeList[edge2].end ;
	}
    }
}
#ifdef DEBUG
for( c = 1 ; c <= numberCells ; c++ ) {
    clptr = cellList[c] ;
    fprintf(fpdebug,"CELL: <%d>\n",c);
    for( ; clptr != (CLBOXPTR) NULL ; clptr = clptr->next ) {
	fprintf(fpdebug,"edge:%d  extraSpace:%d  ",
			clptr->edge, clptr->extraSpace ) ;
	fprintf(fpdebug,"HorV:%d  loc:%d  start:%d  ",
			clptr->HorV, clptr->loc, clptr->start ) ;
	fprintf(fpdebug,"end:%d\n", clptr->end ) ;
    }
}
#endif

return ;
}
