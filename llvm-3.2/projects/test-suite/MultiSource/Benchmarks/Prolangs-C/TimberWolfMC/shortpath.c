/* 
 *   Basically an O( e log n ) Dijkstra's algorithm
 */
#include "route.h"
#include "23tree.h"

extern void tinsert( TNODEPTR *root , int value , int property );
extern void tpop( TNODEPTR *root , TNODEPTR *node , int *value ,
		 int *property);
extern void tdelete( TNODEPTR *root , int value , int property );

void shortpath( int numpnodes )
{

TNODEPTR root , dumnode ;
NNODEPTR nptr ;
GNODEPTR gptr ;
int i , j , pnode , node , D , nextnode , distance ;

for( i = 1 ; i <= numpnodes ; i++ ) {
    pnode = numnodes + i ;
    nptr = pnodeArray[i].nodeList ;

    root = (TNODEPTR) NULL ;
    for( j = 1 ; j <= numnodes + numpnodes ; j++ ) {
	if( j == pnode ) {
	    tinsert( &root , 0 , j ) ;
	    nptr[j].distance = 0 ;
	    nptr[pnode].from = 0 ;
	    continue ;
	}
	nptr[j].distance = VLARGE ;
    }

    for( ; ; ) {
	tpop( &root , &dumnode , &D , &nextnode ) ;
	if( dumnode == (TNODEPTR) NULL ) {
	    break ;
	}
	gptr = gnodeArray[nextnode] ;
	for( ; gptr != (GNODEPTR) NULL ; gptr = gptr->next ) {
	    distance = gptr->length ;
	    node = gptr->node ;
	    if( nptr[node].distance > D + distance ) {
		tdelete( &root , nptr[node].distance , node ) ;
		tinsert( &root , D + distance , node ) ;
		nptr[node].distance = D + distance ;
		nptr[node].from = nextnode ;
	    }
	}

    }
}

return ;
}
