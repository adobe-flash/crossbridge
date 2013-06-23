#include "route.h"
#include "23tree.h"
extern int pnodeAlength ;

extern void tinsert( TNODEPTR *root , int value , int property );
extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
                  int *property);
extern void tdelete( TNODEPTR *root , int value , int property );

int prestrict( short int *ptr, int p , int source , int target )
{

TNODEPTR rootx , dumnode ;
NNODEPTR nptr ;
GNODEPTR gptr , gptr1 ;
int i , j , snode , node , D , nextnode , distance , node1 , node2 ;
int extraD , flag ;

snode = ptr[p] ;
nptr = pnodeArray[ source - numnodes ].nodeList ;

rootx = (TNODEPTR) NULL ;
for( j = 1 ; j <= numnodes + pnodeAlength ; j++ ) {
    if( j == snode ) {
	tinsert( &rootx , 0 , j ) ;
	nptr[snode].temp = 0 ;
	nptr[snode].from2 = 0 ;
	continue ;
    }
    nptr[j].temp = VLARGE ;
}
flag = 0 ;

for( ; ; ) {
    tpop( &rootx , &dumnode , &D , &nextnode ) ;
    if( dumnode == (TNODEPTR) NULL ) {
	break ;
    }
    if( nextnode == target ) {
	flag = 1 ;
	for( ; ; ) {
	    tpop( &rootx , &dumnode , &D , &nextnode ) ;
	    if( dumnode == (TNODEPTR) NULL ) {
		break ;
	    }
	}
	break ;
    }
    gptr = gnodeArray[nextnode] ;
    for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
	if( gptr->inactive == 1 || gptr->einactive == 1 ) {
	    continue ;
	}
	distance = gptr->cost ;
	node = gptr->node ;
	if( nptr[node].temp > D + distance ) {
	    tdelete( &rootx , nptr[node].temp , node ) ;
	    tinsert( &rootx , D + distance , node ) ;
	    nptr[node].temp = D + distance ;
	    nptr[node].from2 = nextnode ;
	}
    }
}
if( flag == 0 ) {
    return( -1 ) ;
}
extraD = 0 ;
for( i = 1 ; i < p ; i++ ) {
    node1 = ptr[i] ;
    node2 = ptr[i + 1] ;
    gptr1 = gnodeArray[node1] ;
    for( ; ; ) {
	if( gptr1->node == node2 ) {
	    extraD += gptr1->cost ;
	    break ;
	}
	gptr1 = gptr1->next ;
    }
}
return( extraD ) ;
}
