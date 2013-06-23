#include "route.h"


void grepair(void)
{
GNODEPTR gptr , g2ptr ;
int i ;

for( i = 1 ; i <= numnodes + maxpnode ; i++ ) {
    gptr = gnodeArray[i] ;
    if( gptr == (GNODEPTR) NULL ) {
	continue ;
    }
    if( i <= numnodes ) {
	if( gptr->node <= numnodes ) {
	    continue ;
	} else {
	    for( ; ; ) {
		if( gptr != (GNODEPTR) NULL ) { 
		    g2ptr = gptr->next ;
		    if( gptr->node > numnodes ) {
			free( gptr ) ;
			gptr = g2ptr ;
		    } else{
			gnodeArray[i] = gptr ;
			break ;
		    }
		} else {
		    gnodeArray[i] = (GNODEPTR) NULL ; 
		    break ;
		}
	    }
	}
	gptr = gnodeArray[i] ;
	for( ; ; ) {
	    if( gptr == (GNODEPTR) NULL ) { 
		break ;
	    }
	    gptr->length = gptr->ilength ;
	    gptr->cost = gptr->ilength ;
	    gptr = gptr->next ;
	}
    } else {
	while( gptr != (GNODEPTR) NULL ) { 
	    g2ptr = gptr->next ;
	    free( gptr ) ;
	    gptr = g2ptr ;
	}
	gnodeArray[i] = (GNODEPTR) NULL ; 
    }
}
return ;
}
