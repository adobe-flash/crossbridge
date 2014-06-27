#include "route.h"

void prepair( int numpnodes )
{

int i , j ;
LIST2PTR lptr , l2ptr ;

for( i = 1 ; i <= numpnodes ; i++ ) {
    pnodeArray[i].eptr = 0 ;
    for( j = 1 ; j <= numnodes + numpnodes ; j++ ) {
	pnodeArray[i].nodeList[j].temp = 0 ;
	pnodeArray[i].nodeList[j].distance = 0 ;
	pnodeArray[i].nodeList[j].from = 0 ;
	pnodeArray[i].nodeList[j].from2 = 0 ;
    }
    if( (lptr = pnodeArray[i].equiv) != (LIST2PTR) NULL ) {
	do {
	    l2ptr = lptr->next ;
	    free( lptr ) ;
	    lptr = l2ptr ;
	} while( lptr != (LIST2PTR) NULL ) ;
	pnodeArray[i].equiv = (LIST2PTR) NULL ;
    }
}

return ;
}
