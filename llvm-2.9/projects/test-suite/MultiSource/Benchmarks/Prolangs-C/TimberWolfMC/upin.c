#include "custom.h"
extern double Tsave ;

int upin( CELLBOXPTR acellptr , int UCpin , int seq , int firstNewSite , 
      int lastNewSite )
{

DIMBOXPTR dimptr ;
NETBOXPTR netptr ;
TERMNETSPTR tmptr ;
TILEBOXPTR atileptr ;
UNCOMBOX *UCptr ;
LOCBOX *SLptr ;
CONTENTBOX *SCptr ;

int tm , site , cost , newOverFill , capacity , count ;
int x , y , newsite ;
int excess ; 
int axcenter , aycenter ;
double Tscale ;

Tscale = Tsave / 100000.0 ;

axcenter = acellptr->xcenter ;
aycenter = acellptr->ycenter ;
atileptr = acellptr->config[ acellptr->orient ] ;

UCptr = acellptr->unComTerms   ;
SCptr = acellptr->siteContent  ;
SLptr = atileptr->siteLocArray ;

newOverFill = overfill ;
/*
    First we must refresh the 'newContents' field of the
    array pointed to by SCptr
*/
for( site = firstNewSite ; site <= lastNewSite ; site++ ) {
    SCptr[ site ].newContents = SCptr[ site ].contents ;
}
for( site = UCptr[ UCpin ].site ; 
	    site <= UCptr[ UCpin + seq - 1 ].site ; site++ ) {
    SCptr[ site ].newContents = SCptr[ site ].contents ;
}

for( tm = UCpin ; tm < UCpin + seq ; tm++ ) {
    site = UCptr[ tm ].site ;
    excess = SCptr[ site ].newContents-- - SCptr[ site ].capacity ;
    if( excess > 0 ) {
	if( excess == 1 ) {
	    newOverFill -= (int)( Tscale * 10.0 ) ; 
	} else {
	    newOverFill -= (int)( Tscale * 10.0 * 
			( (double)(excess * excess)) ) - 
			(int)( Tscale * 10.0 * 
			( (double)((excess - 1) * (excess - 1)))) ;
	}
    }
}

site = firstNewSite - 1 ;
capacity = 0 ;
count = 0 ;
for( tm = UCpin ; tm < UCpin + seq ; tm++ ) {
    count++ ;
    if( count > capacity ) {
	site++ ;
	count = 1 ;
	capacity = SCptr[ site ].capacity ;
    } 
    excess = ++SCptr[ site ].newContents - capacity ;
    if( excess > 0 ) {
	if( excess == 1 ) {
	    newOverFill += (int)( Tscale * 10.0 ) ; 
	} else {
	    newOverFill += (int)( Tscale * 10.0 * 
			( (double)(excess * excess)) ) - 
			(int)( Tscale * 10.0 * 
			( (double)((excess - 1) * (excess - 1)))) ;
	}
    }
    UCptr[tm].newsite = site ;
}

for( tm = UCpin ; tm < UCpin + seq ; tm++ ) {
    newsite = UCptr[ tm ].newsite ;
    x = SLptr[ newsite ].xpos + axcenter ;
    y = SLptr[ newsite ].ypos + aycenter ;

    tmptr = termarray[ UCptr[tm].terminal ] ;
    netarray[ tmptr->net ]->flag = 1 ;
    tmptr->termptr->flag = 1 ;
    tmptr->termptr->newx = x ;
    tmptr->termptr->newy = y ;
}

cost = funccost ;

for( tm = UCpin ; tm < UCpin + seq ; tm++ ) {
    tmptr = termarray[ UCptr[tm].terminal ] ;
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


if( ( cost + newOverFill <= funccost + overfill ) || 
	(exp( (double)( funccost + overfill - cost - newOverFill) /
	T) > ( (double) RAND / (double)0x7fffffff ) ) ){

    for( tm = UCpin ; tm < UCpin + seq ; tm++ ) {
	tmptr = termarray[ UCptr[tm].terminal ] ;
	dimptr = netarray[ tmptr->net ] ;

	dimptr->xmin = dimptr->newxmin ;
	dimptr->xmax = dimptr->newxmax ;
	dimptr->ymin = dimptr->newymin ;
	dimptr->ymax = dimptr->newymax ;

	tmptr->termptr->xpos = tmptr->termptr->newx ;
	tmptr->termptr->ypos = tmptr->termptr->newy ;
    }
    for( site = firstNewSite ; site <= lastNewSite ; site++ ) {
	SCptr[ site ].contents = SCptr[ site ].newContents ;
    }
    for( site = UCptr[ UCpin ].site ; 
		site <= UCptr[ UCpin + seq - 1 ].site ; site++ ) {
	SCptr[ site ].contents = SCptr[ site ].newContents ;
    }
    for( tm = UCpin ; tm < UCpin + seq ; tm++ ) {
	UCptr[ tm ].site = UCptr[ tm ].newsite ;
    }

    funccost = cost ;
    overfill = newOverFill ;
    return (1) ;
} else {
    return (0) ;
}
}
