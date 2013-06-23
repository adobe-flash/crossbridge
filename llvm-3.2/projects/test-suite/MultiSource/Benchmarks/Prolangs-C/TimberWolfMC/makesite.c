#include "custom.h"
typedef struct kbox {
    int cap ;
    int HV ;
    int sp ;
    int x ;
    int y ;
}
KBOX ,
*KBOXPTR ;

extern KBOXPTR kArray ;
double val1 , val2 ;

int Vside( int cell , int x , int yy1 , int yy2 , int flag )
{

double aspFactor , aspect , aspLB ;
int worstLen , pPinLocs , k , length , base ;
int TotSites ;
double siteSpace ;

length = ABS(yy2 - yy1) ;
aspect = cellarray[ cell ]->aspect ;
aspLB  = cellarray[ cell ]->aspLB ;
if( aspLB + 0.01 > cellarray[ cell]->aspUB ) {
    aspFactor = 1.0 ;
} else {
    aspFactor = sqrt( aspLB / aspect ) ;
}
worstLen = (int) (aspFactor * (double) length) ;
if( (double)(worstLen + 1) / aspFactor - (double) length <
		(double) length - (double) worstLen / aspFactor ) {
    worstLen++ ;
}

pPinLocs = worstLen / pinSpacing - 1 ;
if( flag ) {
    if( pPinLocs > MaxSites ) {
	for( k = 1 ; k <= MaxSites ; k++ ) {
	    kArray[ k ].cap = 0 ;
	    kArray[ k ].HV = 0 ;
	    kArray[ k ].sp = 0 ;
	    kArray[ k ].x = 0 ;
	    kArray[ k ].y = 0 ;
	}
	base = pPinLocs / MaxSites ;
	for( k = 1 ; k <= MaxSites ; k++ ) {
	    kArray[ k ].cap += base ;
	}
	for( k = 1 ; k <= pPinLocs % MaxSites ; k++ ) {
	    kArray[ k ].cap++ ;
	}
    } else {  /* prepare pPinLocs sites */
	for( k = 1 ; k <= pPinLocs ; k++ ) {
	    kArray[ k ].cap = 0 ;
	    kArray[ k ].HV = 0 ;
	    kArray[ k ].sp = 0 ;
	    kArray[ k ].x = 0 ;
	    kArray[ k ].y = 0 ;
	}
	for( k = 1 ; k <= pPinLocs ; k++ ) {
	    kArray[ k ].cap = 1 ;
	}
    }
}

/*
    TotSites = minimum of ( MaxSites and pPinLocs ) 
*/
if( pPinLocs <= MaxSites ) {
    TotSites = pPinLocs ;
} else {
    TotSites = MaxSites ;
}
siteSpace = (double) length / (double) (TotSites + 1) ;

/* 
    Suppose we encountered coordinates yy1 and yy2 for a given
    vertical side, and of course that the x-coordinate is simply x.
    ( yy1 < yy2 is required )
*/

if( flag ) {
    if( yy2 > yy1 ) {
	for( k = 1 ; k <= TotSites ; k++ ) {
	    kArray[ k ].x = x ;
	    val1 = (k + 1) * siteSpace ;
	    val2 = k * siteSpace ;
	    kArray[ k ].y = ROUND( val2 ) + yy1 ;
	    kArray[ k ].sp = ROUND( val1 ) - ROUND( val2 ) ;
	    kArray[ k ].HV = 1 ;
	}
    } else {
	for( k = 1 ; k <= TotSites ; k++ ) {
	    kArray[ k ].x = x ;
	    val1 = (k + 1) * siteSpace ;
	    val2 = k * siteSpace ;
	    kArray[ k ].y = yy1 - ROUND( val2 ) ;
	    kArray[ k ].sp = ROUND( val1 ) - ROUND( val2 ) ;
	    kArray[ k ].HV = 1 ;
	}
    }
}

/*
 *
 *   --- BIG NOTE ---
 *
 *   Any fixed terminals intersecting a site cause the contents
 *   of that site to be incremented by one. This will inhibit
 *   sequences from passing on thru. ( try to remember to
 *   get the check pointer to reflect this fact )
 */

return( TotSites ) ;
}




int Hside( int cell , int xx1 , int xx2 , int y , int flag )
{

double aspFactor , aspect , aspUB ;
int worstLen , pPinLocs , k , length , base ;
int TotSites ;
double siteSpace ;

length = ABS(xx2 - xx1) ;
aspect = cellarray[ cell ]->aspect ;
aspUB  = cellarray[ cell ]->aspUB ;
if( cellarray[ cell ]->aspLB + 0.01 > aspUB ) {
    aspFactor = 1.0 ;
} else {
    aspFactor = sqrt( aspect / aspUB ) ;
}
worstLen = (int) (aspFactor * (double) length) ;
if( (double)(worstLen + 1) / aspFactor - (double) length <
		(double) length - (double) worstLen / aspFactor ) {
    worstLen++ ;
}

pPinLocs = worstLen / pinSpacing - 1 ;
if( flag ) {
    if( pPinLocs > MaxSites ) {
	for( k = 1 ; k <= MaxSites ; k++ ) {
	    kArray[ k ].cap = 0 ;
	    kArray[ k ].HV = 0 ;
	    kArray[ k ].sp = 0 ;
	    kArray[ k ].x = 0 ;
	    kArray[ k ].y = 0 ;
	}
	base = pPinLocs / MaxSites ;
	for( k = 1 ; k <= MaxSites ; k++ ) {
	    kArray[ k ].cap += base ;
	}
	for( k = 1 ; k <= pPinLocs % MaxSites ; k++ ) {
	    kArray[ k ].cap++ ;
	}
    } else {  /* prepare pPinLocs sites */
	for( k = 1 ; k <= pPinLocs ; k++ ) {
	    kArray[ k ].cap = 0 ;
	    kArray[ k ].HV = 0 ;
	    kArray[ k ].sp = 0 ;
	    kArray[ k ].x = 0 ;
	    kArray[ k ].y = 0 ;
	}
	for( k = 1 ; k <= pPinLocs ; k++ ) {
	    kArray[ k ].cap = 1 ;
	}
    }
}

/*
    TotSites = minimum of ( MaxSites and pPinLocs ) 
*/
if( pPinLocs <= MaxSites ) {
    TotSites = pPinLocs ;
} else {
    TotSites = MaxSites ;
}
siteSpace = (double) length / (double) (TotSites + 1) ;

/* 
   Suppose we encountered coordinates xx1 and xx2 for a given
   horizontal side, and of course that the y-coordinate is simply y.
*/

if( flag ) {
    if( xx2 > xx1 ) {
	for( k = 1 ; k <= TotSites ; k++ ) {
	    kArray[ k ].y = y ;
	    val1 = (k + 1) * siteSpace ;
	    val2 = k * siteSpace ;
	    kArray[ k ].x = ROUND( val2 ) + xx1 ;
	    kArray[ k ].sp = ROUND( val1 ) - ROUND( val2 ) ;
	    kArray[ k ].HV = 0 ;
	}
    } else {
	for( k = 1 ; k <= TotSites ; k++ ) {
	    kArray[ k ].y = y ;
	    val1 = (k + 1) * siteSpace ;
	    val2 = k * siteSpace ;
	    kArray[ k ].x = xx1 - ROUND( val2 ) ;
	    kArray[ k ].sp = ROUND( val1 ) - ROUND( val2 ) ;
	    kArray[ k ].HV = 0 ;
	}
    }
}

/*
 *
 *   --- BIG NOTE ---
 *
 *   Any fixed terminals intersecting a site cause the contents
 *   of that site to be incremented by one. This will inhibit
 *   sequences from passing on thru. ( try to remember to
 *   get the check pointer to reflect this fact )
 */

return( TotSites ) ;
}
