#include "port.h"
#include "custom.h"

void wirecosts( int *withPads , int *withOutPads )
{

DIMBOXPTR dimptr ;
NETBOXPTR netptr ;

int x , y , xmin , xmax , ymin , ymax , net ;

*withPads = 0 ;
*withOutPads = 0 ;

for( net = 1 ; net <= numnets ; net++ ) {
    dimptr =  netarray[net] ;
    netptr = dimptr->netptr ;

    xmin = 0 ;
    xmax = 0 ;
    ymin = 0 ;
    ymax = 0 ;
    for( ; ; netptr = netptr->nextterm ) {
	if( netptr == NETNULL ) {
	    break ;
	}
	if( netptr->skip == 1 ) {
	    continue ;
	}
	xmin = xmax = netptr->xpos ;
	ymin = ymax = netptr->ypos ;
	netptr = netptr->nextterm ;
	break ;
    }
    for( ; netptr != NETNULL ; netptr = netptr->nextterm ) {
	if( netptr->skip == 1 ) {
	    continue ;
	}
	x = netptr->xpos ;
	y = netptr->ypos ;

	if( x < xmin ) {
	    xmin = x ;
	} else if( x > xmax ) {
	    xmax = x ;
	}
	if( y < ymin ) {
	    ymin = y ;
	} else if( y > ymax ) {
	    ymax = y ;
	}
    }
    *withOutPads += (xmax - xmin) + (ymax - ymin) ;

    dimptr =  netarray[net] ;
    netptr = dimptr->netptr ;
    xmin = 0 ;
    xmax = 0 ;
    ymin = 0 ;
    ymax = 0 ;
    if( netptr != NETNULL ) {
	xmin = xmax = netptr->xpos ;
	ymin = ymax = netptr->ypos ;
	netptr = netptr->nextterm ;
    }
    for( ; netptr != NETNULL ; netptr = netptr->nextterm ) {
	x = netptr->xpos ;
	y = netptr->ypos ;

	if( x < xmin ) {
	    xmin = x ;
	} else if( x > xmax ) {
	    xmax = x ;
	}
	if( y < ymin ) {
	    ymin = y ;
	} else if( y > ymax ) {
	    ymax = y ;
	}
    }
    *withPads += (xmax - xmin) + (ymax - ymin) ;
}
return ;

}
