#include "route.h"

int identify2( int *node1 , int *node2 , int distance , int *bound1 , 
	      int *bound2 , GNODEPTR *gpptr );

void loadpg( int net , int totalnodes )
{

int i , node1 , node2 , pnode , PorE , distance , splitL , capacity ;
int savepnode , bound1 , bound2 , j , pnode1 , pnode2 ;
QUADPTR qptr ;
GNODEPTR gptr , g2ptr , gptr1 , gptr2 ;
LIST2PTR lptr ;


qptr = pinlist ;
for( i = 1 ; i <= totalnodes ; i++ ) {
    node1 = qptr->node1 ;
    node2 = qptr->node2 ;
    distance = qptr->distance ;
    PorE = qptr->PorE ;
    j = identify2( &node1, &node2, distance, &bound1, &bound2, &gptr );
    if( j == 0 ) {
	fprintf(fpo,"pin number: %d of net: %d was supposed to ",
						    i , net ) ;
	fprintf(fpo,"lie between nodes: %d and %d\n", node1, node2);
	fprintf(fpo,"However, the graph doesn't have an edge ");
	fprintf(fpo,"between these two nodes\n");
	exit(0);
    } else if( j == 1 ) {
	fprintf(fpo,"pin number: %d of net: %d ", i , net ) ;
	fprintf(fpo,"specified to lie between nodes: %d and %d\n",
						node1 , node2 ) ;
	fprintf(fpo,"is not within the scope of this channel\n");
	exit(0);
    }
    splitL = bound2 - bound1 ;
    capacity = gptr->capacity ;
    if( node1 <= numnodes && node2 <= numnodes ) {
	gptr->length = VLARGE ;
	gptr->cost = VLARGE ;
	gptr = gnodeArray[node2] ;
	while( gptr->node != node1 ) {
	    gptr = gptr->next ;
	}
	gptr->length = VLARGE ;
	gptr->cost = VLARGE ;
    } else {
	if( gnodeArray[node1] == gptr ) {
	    gnodeArray[node1] = gptr->next ;
	    free( gptr ) ;
	} else {
	    g2ptr = gnodeArray[node1] ;
	    while( g2ptr->next != gptr ) {
		g2ptr = g2ptr->next ;
	    }
	    g2ptr->next = gptr->next ;
	    free( gptr ) ;
	}
	if( gnodeArray[node2]->node == node1 ) {
	    gptr = gnodeArray[node2] ;
	    gnodeArray[node2] = gnodeArray[node2]->next ;
	    free( gptr ) ;
	} else {
	    g2ptr = gnodeArray[node2] ;
	    while( g2ptr->next->node != node1 ) {
		g2ptr = g2ptr->next ;
	    }
	    gptr = g2ptr->next ;
	    g2ptr->next = gptr->next ;
	    free( gptr ) ;
	}
    }
    pnode = i + numnodes ;
    if( PorE == 1 ) {
	savepnode = i ;
	pnodeArray[i].eptr = 0 ;
    } else {
	lptr = pnodeArray[savepnode].equiv ;
	pnodeArray[i].eptr = savepnode ;
	pnodeArray[savepnode].equiv = (LIST2PTR) malloc(sizeof(LIST2));
	pnodeArray[savepnode].equiv->next = lptr ;
	pnodeArray[savepnode].equiv->node = i ;
    }
    distance -= bound1 ;
    gnodeArray[pnode] = (GNODEPTR) malloc( sizeof(GNODE) ) ;
    gnodeArray[pnode]->node = node1 ;
    gnodeArray[pnode]->length = distance ;
    gnodeArray[pnode]->ilength = distance ;
    gnodeArray[pnode]->cost = distance ;
    gnodeArray[pnode]->capacity = capacity ;
    gnodeArray[pnode]->inactive = 0 ;
    gnodeArray[pnode]->einactive = 0 ;
    gnodeArray[pnode]->next = (GNODEPTR) malloc( sizeof(GNODE) ) ;
    gnodeArray[pnode]->next->node = node2 ;
    gnodeArray[pnode]->next->length = splitL - distance ;
    gnodeArray[pnode]->next->ilength = splitL - distance ;
    gnodeArray[pnode]->next->cost = splitL - distance ;
    gnodeArray[pnode]->next->capacity = capacity ;
    gnodeArray[pnode]->next->inactive = 0 ;
    gnodeArray[pnode]->next->einactive = 0 ;
    gnodeArray[pnode]->next->next = (GNODEPTR) NULL ;

    gptr = gnodeArray[node1] ;
    gnodeArray[node1] = (GNODEPTR) malloc( sizeof(GNODE) ) ;
    gnodeArray[node1]->next = gptr ;
    gnodeArray[node1]->node = pnode ;
    gnodeArray[node1]->ilength = distance ;
    gnodeArray[node1]->length = distance ;
    gnodeArray[node1]->cost = distance ;
    gnodeArray[node1]->capacity = capacity ;
    gnodeArray[node1]->inactive = 0 ;
    gnodeArray[node1]->einactive = 0 ;

    gptr = gnodeArray[node2] ;
    gnodeArray[node2] = (GNODEPTR) malloc( sizeof(GNODE) ) ;
    gnodeArray[node2]->next = gptr ;
    gnodeArray[node2]->node = pnode ;
    gnodeArray[node2]->ilength = splitL - distance ;
    gnodeArray[node2]->length = splitL - distance ;
    gnodeArray[node2]->cost = splitL - distance ;
    gnodeArray[node2]->capacity = capacity ;
    gnodeArray[node2]->inactive = 0 ;
    gnodeArray[node2]->einactive = 0 ;

    qptr = qptr->next ;
}
for( i = 1 ; i <= totalnodes ; i++ ) {
    pnode1 = i + numnodes ;
    gptr = gnodeArray[pnode1] ;
    /* Mitch added check if gptr is null */
    for( ; gptr != (GNODEPTR) NULL ; gptr = (gptr ? gptr->next : gptr)) {
	pnode2 = gptr->node ;
	j = pnode2 - numnodes ;
	if( pnode2 <= numnodes || j < i ) {
	    continue ;
	}
	if( (pnodeArray[i].eptr != 0) && (pnodeArray[j].eptr != 0) ) {
	    if( pnodeArray[i].eptr != pnodeArray[j].eptr ) {
		continue ;
	    }
	} else if( pnodeArray[i].eptr == 0 && pnodeArray[j].eptr == 0){
	    continue ;
	} else {
	    if( pnodeArray[i].eptr != 0 ) {
		if( pnodeArray[i].eptr != j ) {
		    continue ;
		}
	    } else {
		if( pnodeArray[j].eptr != i ) {
		    continue ;
		}
	    }
	}
	gptr1 = gnodeArray[pnode1] ;
	gptr2 = gnodeArray[pnode2] ;
	gptr = gptr1 ;
	/* Mitch added check if gptr is null */
	for( ; gptr; ) {
	    if( gptr->node == pnode2 ) {
		gptr->cost = VLARGE ;
		break ;
	    }
	    gptr = gptr->next ;
	}
	gptr = gptr2 ;
	/* Mitch added check if gptr is null */
	for( ; gptr; ) {
	    if( gptr->node == pnode1 ) {
		gptr->cost = VLARGE ;
		break ;
	    }
	    gptr = gptr->next ;
	}
    }
}

return ;
}



int identify2( int *node1 , int *node2 , int distance , int *bound1 , 
                int *bound2 , GNODEPTR *gpptr )
{

GNODEPTR gptr1 , gptr ;
int c , count , i , prev , temp , limit ;

gptr1 = gnodeArray[ *node1 ] ;

for( i = 1 ; i <= 4 ; i++ ) {
    gtrace[i][0] = 1 ;
    gtrace[i][1] = *node1 ;
}
count = 0 ;
for( gptr = gptr1 ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
    if( gptr->length >= VLARGE ) {
	continue ;
    }
    count++ ;
    gtrace[count][ ++gtrace[count][0] ] = gptr->node ;
}
for( c = 1 ; c <= count ; c++ ) {
    while( gtrace[c][ gtrace[c][0] ] > numnodes ) {
	prev = gtrace[c][ gtrace[c][0] - 1 ] ;
	gptr = gnodeArray[ gtrace[c][ gtrace[c][0] ] ] ;
	if( gptr->node == prev ) {
	    gptr = gptr->next ;
	}
	gtrace[c][ ++gtrace[c][0] ] = gptr->node ;
    }
}
for( c = 1 ; c <= count ; c++ ) {
    if( gtrace[c][ gtrace[c][0] ] == *node2 ) {
	break ;
    }
}
if( c > count ) {
    return(0);
}
temp = 0 ;
limit = gtrace[c][0] ;
for( i = 1 ; i < limit ; i++ ) {
    gptr = gnodeArray[ gtrace[c][i] ] ;
    while( gptr->node != gtrace[c][i + 1] ) {
	gptr =  gptr->next ;
    }
    prev = temp ;
    temp += gptr->length ;
    if( temp >= distance ) {
	break ;
    }
}
if( i >= limit ) {
    return(1);
}
*node1 = gtrace[c][i] ;
*node2 = gtrace[c][i + 1] ;
*bound1 = prev ;
*bound2 = temp ;
*gpptr = gptr ;
return(2);
}
