#include "custom.h"

int newOrient( CELLBOXPTR cellptr , int range )
{

int incidence , count , i , orient ;

orient = cellptr->orient ;
if( range == 4 ) {
    if( orient >= 4 ) {
	count = 0 ;
	for( i = 0 ; i <= 3 ; i++ ) {
	    if( cellptr->orientList[i] == 1 ) {
		count++ ;
	    }
	}
	if( count == 0 ) {
	    return( -1 ) ;
	}
	do {
	    incidence = (int) ( (double) count * 
			      ( (double) RAND / 
			      (double) 0x7fffffff ) ) + 1 ;
	} while( incidence == count + 1 ) ;

	count = 0 ;
	for( i = 0 ; i <= 3 ; i++ ) {
	    if( cellptr->orientList[i] == 1 ) {
		if( ++count == incidence ) {
		    return( i ) ;
		}
	    }
	}
	
    } else {
	count = 0 ;
	for( i = 4 ; i <= 7 ; i++ ) {
	    if( cellptr->orientList[i] == 1 ) {
		count++ ;
	    }
	}
	if( count == 0 ) {
	    return( -1 ) ;
	}
	do {
	    incidence = (int) ( (double) count * 
			     ( (double) RAND / 
			     (double) 0x7fffffff ) ) + 1 ;
	} while( incidence == count + 1 ) ;

	count = 0 ;
	for( i = 4 ; i <= 7 ; i++ ) {
	    if( cellptr->orientList[i] == 1 ) {
		if( ++count == incidence ) {
		    return( i ) ;
		}
	    }
	}
    }
} else {
    count = 0 ;
    for( i = 0 ; i <= 7 ; i++ ) {
	if( i == orient ) {
	    continue ;
	}
	if( cellptr->orientList[i] == 1 ) {
	    count++ ;
	}
    }
    if( count == 0 ) {
	return( -1 ) ;
    }
    do {
	incidence = (int) ( (double) count * 
			  ( (double) RAND / 
			  (double) 0x7fffffff ) ) + 1 ;
    } while( incidence == count + 1 ) ;

    count = 0 ;
    for( i = 0 ; i <= 7 ; i++ ) {
	if( i == orient ) {
	    continue ;
	}
	if( cellptr->orientList[i] == 1 ) {
	    if( ++count == incidence ) {
		return( i ) ;
	    }
	}
    }
}
/*NOTREACHED*/
}
