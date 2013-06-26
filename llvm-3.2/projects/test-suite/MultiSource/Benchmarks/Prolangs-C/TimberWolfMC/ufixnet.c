#include "custom.h"

int ufixnet( TERMBOXPTR termsptr )
{

DIMBOXPTR dimptr ;
NETBOXPTR netptr ;
TERMNETSPTR tmptr ;
TERMBOXPTR termptr ;

int cost ;
int x , y ;


cost = 0 ;
for( termptr = termsptr ; termptr != TERMNULL ; termptr =
					    termptr->nextterm ) {
    tmptr = termarray[ termptr->terminal ];
    dimptr = netarray[ tmptr->net ] ;
    if( dimptr->flag == 0 || dimptr->skip == 1 ) {
	continue ;
    }
    dimptr->flag = 0 ;
    netptr = dimptr->netptr ;
    for( ; ; netptr = netptr->nextterm ) {
	if( netptr == NETNULL ) {
	    break ;
	}
	if( netptr->skip == 1 ) {
	    continue ;
	}
	if( netptr->flag == 1 ) {
	    dimptr->newxmin = dimptr->newxmax = netptr->newx ;
	    dimptr->newymin = dimptr->newymax = netptr->newy ;
	    netptr->flag = 0 ;
	} else {
	    dimptr->newxmin = dimptr->newxmax = netptr->xpos ;
	    dimptr->newymin = dimptr->newymax = netptr->ypos ;
	}
	netptr = netptr->nextterm ;
	break ;
    }
    for( ; netptr != NETNULL ; netptr = netptr->nextterm ) {
	if( netptr->skip == 1 ) {
	    continue ;
	}
	if( netptr->flag == 1 ) {
	    x = netptr->newx ;
	    y = netptr->newy ;
	    netptr->flag = 0 ;
	} else {
	    x = netptr->xpos ;
	    y = netptr->ypos ;
	}
	if( x < dimptr->newxmin ) {
	    dimptr->newxmin = x ;
	} else if( x > dimptr->newxmax ) {
	    dimptr->newxmax = x ;
	}
	if( y < dimptr->newymin ) {
	    dimptr->newymin = y ;
	} else if( y > dimptr->newymax ) {
	    dimptr->newymax = y ;
	}
    } 
    cost += ((int)( dimptr->Hweight *
	    (double)(dimptr->newxmax - dimptr->newxmin))) - 
	    ((int)( dimptr->Hweight *
	    (double)(dimptr->xmax - dimptr->xmin))) +
	    ((int)( dimptr->Vweight *
	    (double)( dimptr->newymax - dimptr->newymin))) -
	    ((int)( dimptr->Vweight *
	    (double)( dimptr->ymax - dimptr->ymin)));
}
return( cost ) ;
}
