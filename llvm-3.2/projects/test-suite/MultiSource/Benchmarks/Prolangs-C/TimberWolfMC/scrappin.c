#include "custom.h"

void scrappin(void)
{

DIMBOXPTR dimptr ;
NETBOXPTR netptr ;

int net ;


for( net = 1 ; net <= numnets ; net++ ) {
    dimptr = netarray[net] ;
    netptr = dimptr->netptr ;
    for( ; netptr != NETNULL ; netptr = netptr->nextterm ) {
	if( netptr->cell > numcells ) {
	    netptr->skip = 1 ;
	}
    }
}

return ;
}
