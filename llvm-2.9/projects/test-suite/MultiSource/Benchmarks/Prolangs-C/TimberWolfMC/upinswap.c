#include "custom.h"

int upinswap(CELLBOXPTR acellptr,int pin1,int pin2,int site1,int site2 )
{

DIMBOXPTR dimptr ;
NETBOXPTR netptr ;
TERMNETSPTR tmptr ;
TILEBOXPTR atileptr ;
UNCOMBOX *UCptr ;
LOCBOX *SLptr ;
int cost , temp ;
int x , y ;
int axcenter , aycenter ;

axcenter = acellptr->xcenter ;
aycenter = acellptr->ycenter ;
atileptr = acellptr->config[ acellptr->orient ] ;
UCptr = acellptr->unComTerms   ;
SLptr = atileptr->siteLocArray ;

x = SLptr[site2].xpos + axcenter ;
y = SLptr[site2].ypos + aycenter ;
tmptr = termarray[ UCptr[pin1].terminal ] ;
netarray[tmptr->net]->flag = 1 ;
tmptr->termptr->flag = 1 ;
tmptr->termptr->newx = x ;
tmptr->termptr->newy = y ;


x = SLptr[site1].xpos + axcenter ;
y = SLptr[site1].ypos + aycenter ;
tmptr = termarray[ UCptr[pin2].terminal ] ;
netarray[tmptr->net]->flag = 1 ;
tmptr->termptr->flag = 1 ;
tmptr->termptr->newx = x ;
tmptr->termptr->newy = y ;


cost = funccost ;


tmptr = termarray[ UCptr[pin1].terminal ] ;
dimptr = netarray[ tmptr->net ] ;
if( dimptr->flag != 0 && dimptr->skip != 1 ) {
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

tmptr = termarray[ UCptr[pin2].terminal ] ;
dimptr = netarray[ tmptr->net ] ;
if( dimptr->flag != 0 && dimptr->skip != 1 ) {
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

if( ( cost <= funccost ) || (exp( (double)( funccost - cost ) /
		    T) > ( (double) RAND / (double)0x7fffffff ) ) ){

    
    tmptr = termarray[ UCptr[pin1].terminal ] ;
    dimptr = netarray[ tmptr->net ] ;
    dimptr->xmin = dimptr->newxmin ;
    dimptr->xmax = dimptr->newxmax ;
    dimptr->ymin = dimptr->newymin ;
    dimptr->ymax = dimptr->newymax ;
    tmptr->termptr->xpos = tmptr->termptr->newx ;
    tmptr->termptr->ypos = tmptr->termptr->newy ;
    
    tmptr = termarray[ UCptr[pin2].terminal ] ;
    dimptr = netarray[ tmptr->net ] ;
    dimptr->xmin = dimptr->newxmin ;
    dimptr->xmax = dimptr->newxmax ;
    dimptr->ymin = dimptr->newymin ;
    dimptr->ymax = dimptr->newymax ;
    tmptr->termptr->xpos = tmptr->termptr->newx ;
    tmptr->termptr->ypos = tmptr->termptr->newy ;

    temp = UCptr[pin1].terminal ;
    UCptr[pin1].terminal = UCptr[pin2].terminal ;
    UCptr[pin2].terminal = temp ;

    temp = UCptr[pin1].site ;
    UCptr[pin1].site = UCptr[pin2].site ;
    UCptr[pin2].site = temp ;

    temp = UCptr[pin1].sequence ;
    UCptr[pin1].sequence = UCptr[pin2].sequence ;
    UCptr[pin2].sequence = temp ;

    funccost = cost ;
    return (1) ;
} else {
    return (0) ;
}
}
