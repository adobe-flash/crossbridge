#include "custom.h"

void scrapnet(void)
{

DIMBOXPTR dimptr ;
NETBOXPTR netptr ;

int *vector , criticalMass , count , temp , net , cell ;

vector = (int *) malloc( (1 + numcells) * sizeof(int) ) ;
temp = (int) ( 0.70 * (double) numcells ) ;
criticalMass = ( temp > 8 ) ? temp : 8 ;

for( net = 1 ; net <= numnets ; net++ ) {
    for( cell = 1 ; cell <= numcells ; cell++ ) {
	vector[cell] = 0 ;
    }
    count = 0 ;
    dimptr = netarray[net] ;
    netptr = dimptr->netptr ;
    for( ; netptr != NETNULL ; netptr = netptr->nextterm ) {
	cell = netptr->cell ;
	if( cell <= numcells ) {
	    if( vector[cell] == 0 ) {
		vector[cell] = 1 ;
		count++ ;
	    }
	}
    }
    if( count >= criticalMass ) {
	dimptr->skip = 1 ;
    }
}
free( vector ) ;

return ;
}
